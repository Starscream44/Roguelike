// @file Logger.h

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine
{
enum class LogLevel
{
    INFO,
    WARNING,
    ERROR
};

class LogSink
{
   public:
    virtual ~LogSink() = default;
    virtual void log(LogLevel level, const std::string& message) = 0;
};

class LogFormatter
{
   public:
    static std::string ToString(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::INFO:
                return "[INFO]";
            case LogLevel::WARNING:
                return "[WARNING]";
            case LogLevel::ERROR:
                return "[ERROR]";
            default:
                return "[UNKNOWN]";
        }
    }
};

class ConsoleSink : public LogSink
{
   public:
    void log(LogLevel level, const std::string& message) override
    {
        std::cout << LogFormatter::ToString(level) << " " << message << std::endl;
    }
};

class FileSink : public LogSink
{
   public:
    explicit FileSink(const std::string& filename)
    {
        std::filesystem::path logPath(filename);
        std::filesystem::create_directories(logPath.parent_path());

        logFile.open(filename, std::ios::app);
    }

    ~FileSink() override
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    void log(LogLevel level, const std::string& message) override
    {
        if (!logFile)
        {
            return;
        }

        logFile << LogFormatter::ToString(level) << " " << message << std::endl;
    }

   private:
    std::ofstream logFile;
};

class Logger
{
   public:
    void addSink(std::shared_ptr<LogSink> sink) { sinks.push_back(sink); }

    void log(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logMutex);

        for (auto& sink : sinks)
        {
            sink->log(level, message);
        }
    }

    void info(const std::string& message) { log(LogLevel::INFO, message); }

    void warn(const std::string& message) { log(LogLevel::WARNING, message); }

    void error(const std::string& message) { log(LogLevel::ERROR, message); }

   private:
    std::vector<std::shared_ptr<LogSink>> sinks;
    std::mutex logMutex;
};

class LoggerRegistry
{
   public:
    static LoggerRegistry& getInstance()
    {
        static LoggerRegistry instance;
        return instance;
    }

    std::shared_ptr<Logger> getLogger(const std::string& name)
    {
        std::lock_guard<std::mutex> lock(registryMutex);

        if (loggers.find(name) != loggers.end())
        {
            return loggers[name];
        }

        return defaultLogger;
    }

    void setDefaultLogger(std::shared_ptr<Logger> logger) { defaultLogger = logger; }

    void registerLogger(const std::string& name, std::shared_ptr<Logger> logger)
    {
        std::lock_guard<std::mutex> lock(registryMutex);
        loggers[name] = logger;
    }

   private:
    std::unordered_map<std::string, std::shared_ptr<Logger>> loggers;
    std::shared_ptr<Logger> defaultLogger;
    std::mutex registryMutex;
};
}  // namespace Engine

#define LOG_INFO(message) ::Engine::LoggerRegistry::getInstance().getLogger("global")->info(message)

#define LOG_WARN(message) ::Engine::LoggerRegistry::getInstance().getLogger("global")->warn(message)

#define LOG_ERROR(message) \
    ::Engine::LoggerRegistry::getInstance().getLogger("global")->error(message)