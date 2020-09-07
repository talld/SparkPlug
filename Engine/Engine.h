#pragma once

#include <Input/Input.h>
#include <Renderer/Renderer.h>



class Engine {
public:
	void boot() {
		Renderer renderer;
		renderer.bootRender();
		renderer.enterMainLoop();
		renderer.terminate();
	}
};