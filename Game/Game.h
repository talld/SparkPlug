#ifndef Game_H
#define Game_H

#include <vector>

#include <Core/Object/Mesh/Mesh.h>
#include <Renderer/Renderer.h>

class Game
{
	std::vector<Mesh> meshes;
	Renderer* renderer;
public:
	Game(Renderer* renderer) {
		this->renderer = renderer;
	}

	Game() {
	}

	~Game() {

	}

	std::vector<Camera*> cameras; //vector containing all created cameras

	void start() {

		Camera* cam = new Camera();
		this->cameras.push_back(cam);
		renderer->bindCamera(cam);

		cam->update();

		std::vector<Vertex> meshVertices1 = {
		{ { -0.1, -0.4, 0.0 },{ 1.0f,0.0f,0.0f } },
	{ { -0.1, 0.4, 0.0 },{ 0.0f, 1.0f, 0.0f } },
	{ { -0.9, 0.4, 0.0 },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.9, -0.4, 0.0 },{ 0.0f, 0.0f, 1.0f } },
		};

		std::vector<Vertex> meshVertices2 = {
			{ { 0.9, -0.3, 0.0 },{ 1.0f,0.0f,0.0f } },
		{ { 0.9, 0.1, 0.0 },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.1, 0.3, 0.0 },{ 0.0f, 0.0f, 1.0f } },
		{ { 0.1, -0.3, 0.0 },{ 0.0f, 0.0f, 1.0f } },
		};

		std::vector<uint32_t> meshIndices = {
			0,1,2,
			2,3,0
		};

		Mesh mesh1;
		renderer->createMesh(&mesh1, &meshVertices1, &meshIndices);
		meshes.push_back(mesh1);
		Mesh mesh2;
		renderer->createMesh(&mesh2, &meshVertices2, &meshIndices);
		meshes.push_back(mesh2);

		renderer->record(&meshes);
	};

	std::vector<Mesh>* getMeshes() {
		return &meshes;
	}

	void terminate() {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i].destroy();
		}
		for(auto camera : cameras){
			delete camera;
		}
	}
};

#endif