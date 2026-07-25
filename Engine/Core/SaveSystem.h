// @file SaveSystem.h

#pragma once

#include <any>
#include <optional>
#include <string>
#include <unordered_map>

namespace Engine
{
// Runtime key-value save store for data that must survive scene restarts.
class SaveSystem
{
   public:
    static SaveSystem* Instance();

    template <typename T>
    void SetValue(const std::string& key, const T& value)
    {
        values[key] = value;
    }

    template <typename T>
    std::optional<T> GetValue(const std::string& key) const
    {
        auto value = values.find(key);

        if (value == values.end())
        {
            return std::nullopt;
        }

        try
        {
            return std::any_cast<T>(value->second);
        }
        catch (const std::bad_any_cast&)
        {
            return std::nullopt;
        }
    }

    bool HasValue(const std::string& key) const;
    void RemoveValue(const std::string& key);
    void Clear();

   private:
    std::unordered_map<std::string, std::any> values;
};
}  // namespace Engine
