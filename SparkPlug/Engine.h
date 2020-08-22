#pragma once

#include "Input.h"
#include "VkRender.h"
#include "Engine.h"

class Engine {
public:
	void boot() {
		VkRender renderer;
		renderer.bootRender();
		renderer.enterMainLoop();
		renderer.terminate();
	}
};