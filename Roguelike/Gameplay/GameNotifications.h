#pragma once

#include <optional>
#include <string>

namespace Roguelike
{
class GameNotifications
{
   public:
    static void Push(const std::string& message);
    static std::optional<std::string> Consume();

   private:
    static std::optional<std::string> message;
};
}  // namespace Roguelike
