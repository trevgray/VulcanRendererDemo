#include <glew.h>
#include <iostream>
#include "Debug.h"
#include "Scene0.h"
#include "MMath.h"
#include "Debug.h"
#include "VulkanRenderer.h"
#include "Camera.h"

#include "Actor.h"

Scene0::Scene0(Renderer *renderer_): 
	Scene(nullptr),renderer(renderer_){
	camera = new Camera();


	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
	if (camera) delete camera;
}

bool Scene0::OnCreate() {
	float aspectRatio;
	int width, height;
	switch (renderer->getRendererType()){
	case RendererType::VULKAN:
		SDL_GetWindowSize(dynamic_cast<VulkanRenderer*>(renderer)->GetWindow(), &width, &height); //probably should set the window size when we create it
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		camera->Perspective(45.0f, aspectRatio, 0.5f, 20.0f);
		camera->SetViewMatrix(MMath::translate(Vec3(0.0f, 0.0f, -5.0f)) * MMath::rotate(0.0f, Vec3(0.0f, 1.0f, 0.0f)));

		globalLights.lightPos[0] = Vec4(10.0f, 0.0f, 0.0f, 0.0f); globalLights.lightPos[1] = Vec4(-10.0f, 0.0f, 0.0f, 0.0f); 
		globalLights.lightPos[2] = Vec4(0.0f, 10.0f, 0.0f, 0.0f); globalLights.lightPos[3] = Vec4(0.0f, -150.0f, 0.0f, 0.0f);

		globalLights.lightColour[0] = Vec4(0.6f, 0.1f, 0.1f, 0.0f); globalLights.lightColour[1] = Vec4(0.1f, 0.1f, 0.6f, 0.0f);
		globalLights.lightColour[2] = Vec4(0.1f, 0.6f, 0.1f, 0.0f); globalLights.lightColour[3] = Vec4(0.0f, 0.0f, 0.0f, 0.0f);


		break;

	case RendererType::OPENGL:
		break;
	}

	return true;
}

void Scene0::HandleEvents(const SDL_Event &sdlEvent) {
	if (sdlEvent.type == SDL_WINDOWEVENT) {
		switch (sdlEvent.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		std::cout << "Change size Event\n";
		float aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
		camera->Perspective(45.0f, aspectRatio, 0.5, 20.0f);
		break;

		}
	}
}

void Scene0::Update(const float deltaTime) {
	static bool textureChange = false;
	static float elapsedTime = 0.0f;
	elapsedTime += deltaTime;
	mariosModelMatrix = MMath::translate(Vec3(0.0f, 0.0f, -3.0f)) * MMath::rotate(elapsedTime * 90.0f, Vec3(0.0f, 1.0f, 0.0f));
	if (elapsedTime > 10.0f && textureChange == false) {
		VulkanRenderer* vRenderer;
		vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
		vRenderer->createTextureImage("./textures/mario_fire.png", vRenderer->actorGraph["0"].image);
		textureChange = true;
		std::cout << "Texture Change" << std::endl;
	}
}

void Scene0::Render() const {
	/*const Vec4 lightPositions[4] { Vec4(10.0f,0.0f,0.0f,0.0f), Vec4(-10.0f,0.0f,0.0f,0.0f), Vec4(0.0f,10.0f,0.0f,0.0f), Vec4(0.0f,-150.0f,0.0f,0.0f) };
	const Vec4 lightColours[4]{ Vec4(0.6f, 0.1f, 0.1f, 0.0f), Vec4(0.1f, 0.1f, 0.6f, 0.0f), Vec4(0.1f, 0.6f, 0.1f, 0.0f), Vec4(0.0f, 0.0f, 0.0f, 0.0f) };*/
	switch (renderer->getRendererType()) {
	case RendererType::VULKAN:
		VulkanRenderer* vRenderer;
		vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(), camera->GetViewMatrix());
		//vRenderer->SetGlobalLightUBO(lightPositions, lightColours);
		vRenderer->SetGlobalLightUBO(globalLights);
		vRenderer->SetMeshPushConstant(mariosModelMatrix);
		vRenderer->Render();
		break;

	case RendererType::OPENGL:
		/// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		/// Draw your scene here
		
		
		glUseProgram(0);
		
		break;
	}
}


void Scene0::OnDestroy() {
	
}
