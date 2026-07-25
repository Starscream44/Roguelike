// @file Engine.h

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "SFML/Graphics.hpp"

namespace Engine
{
class Scene;

class Engine
{
   public:
    Engine(const Engine& app) = delete;
    Engine& operator=(const Engine&) = delete;

    static Engine* Instance();

    void SetScene(Scene* newScene);
    void Run();
    void RequestSceneRestart();

   private:
    Engine();
    ~Engine() = default;

    void ProcessSceneRestart();

    Scene* scene = nullptr;
    bool isSceneRestartRequested = false;
};
}  // namespace Engine
