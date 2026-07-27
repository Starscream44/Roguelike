#include "pch.h"
#include "SaveSystem.h"

namespace Engine
{
SaveSystem* SaveSystem::Instance()
{
    static SaveSystem saveSystem;
    return &saveSystem;
}

bool SaveSystem::HasValue(const std::string& key) const
{
    return values.find(key) != values.end();
}

void SaveSystem::RemoveValue(const std::string& key)
{
    values.erase(key);
}

void SaveSystem::Clear()
{
    values.clear();
}
}  // namespace Engine
