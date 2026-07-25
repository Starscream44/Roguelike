// @file Engine.cpp

#include "pch.h"
#include "Engine.h"

#include "GameWorld.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "Logger.h"

namespace Engine
{
Engine::Engine() {}

Engine* Engine::Instance()
{
    static Engine engine;
    return &engine;
}

void Engine::SetScene(Scene* newScene) { scene = newScene; }

void Engine::RequestSceneRestart() { isSceneRestartRequested = true; }

void Engine::Run()
{
    if (scene == nullptr)
    {
        LOG_ERROR("Engine cannot run because scene is null.");
        return;
    }

    scene->Start();

    sf::RenderWindow& window = RenderSystem::Instance()->GetMainWindow();
    window.setKeyRepeatEnabled(false);

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            GameWorld::Instance()->HandleEvent(event);
        }

        float deltaTime = clock.restart().asSeconds();

        GameWorld::Instance()->Update(deltaTime);
        GameWorld::Instance()->FixedUpdate(deltaTime);

        window.clear();

        GameWorld::Instance()->Render();

        window.display();

        GameWorld::Instance()->LateUpdate();

        ProcessSceneRestart();
    }

    scene->Stop();
}

void Engine::ProcessSceneRestart()
{
    if (!isSceneRestartRequested || scene == nullptr)
    {
        return;
    }

    isSceneRestartRequested = false;
    scene->Restart();
}
}  // namespace Engine
