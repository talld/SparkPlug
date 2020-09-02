#pragma once

#include "Input.h"
#include "Renderer.h"
#include "Engine.h"

class Engine {
public:
	void boot() {
		Renderer renderer;
		renderer.bootRender();
		renderer.enterMainLoop();
		renderer.terminate();
	}
};