#include "pch.h"

#include <cstdlib>
#include <ctime>
#include <memory>

#include "Engine.h"
#include "GameConfig.h"
#include "RenderSystem.h"
#include "Logger.h"
#include "DeveloperLevel.h"

static void SetupLogger()
{
    auto logger = std::make_shared<Engine::Logger>();

    logger->addSink(std::make_shared<Engine::ConsoleSink>());
    logger->addSink(std::make_shared<Engine::FileSink>("Saved/Logs/log.txt"));

    Engine::LoggerRegistry::getInstance().registerLogger("global", logger);
    Engine::LoggerRegistry::getInstance().setDefaultLogger(logger);
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    SetupLogger();

    LOG_INFO("Logger initialized.");
    LOG_INFO("Game started.");

    sf::RenderWindow* window = new sf::RenderWindow(
        sf::VideoMode(Roguelike::GameConfig::WindowWidth, Roguelike::GameConfig::WindowHeight),
        "Roguelike");

    Engine::RenderSystem::Instance()->SetMainWindow(window);

    Roguelike::DeveloperLevel developerLevel;

    Engine::Engine::Instance()->SetScene(&developerLevel);
    Engine::Engine::Instance()->Run();

    LOG_INFO("Game closed.");

    delete window;

    return 0;
}
