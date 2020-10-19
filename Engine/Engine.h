#ifndef Engine_H
#define Engine_H

#include <iostream>

#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Game.h>


class Engine {
public:
	void boot() {
		Renderer* renderer = new Renderer();
		renderer->bootRender();
		Game game(renderer);
		game.start();
		renderer->enterMainLoop();
		game.terminate();
		renderer->terminate();
		delete renderer;
		std::cin.get(); //pause to read error codes
	}
};

#endif