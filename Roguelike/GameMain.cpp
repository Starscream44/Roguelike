// ©2023, XYZ School. All rights reserved.
// Authored by Aleksandr Rybalka (polterageist@gmail.com)

#include <SFML/Graphics.hpp>
#include "Application.h"
#include "Engine.h"


using namespace Roguelike;

int main()
{
	Engine::Engine::Instance()->Run();
	return 0;
}
