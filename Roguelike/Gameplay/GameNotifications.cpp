#include "pch.h"
#include "GameNotifications.h"

namespace Roguelike
{
std::optional<std::string> GameNotifications::message = std::nullopt;

void GameNotifications::Push(const std::string& newMessage) { message = newMessage; }

std::optional<std::string> GameNotifications::Consume()
{
    std::optional<std::string> result = message;
    message.reset();
    return result;
}
}  // namespace Roguelike
