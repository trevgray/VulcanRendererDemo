#ifndef RENDERER_H
#define RENDERER_H
#include <SDL.h>
#include <string>
enum class RendererType {
	NONE,
	OPENGL,
	VULKAN,
	DIRECTX11,
	DIRECTX12
};

class Renderer {
public:
	Renderer():renderer(nullptr),rendererType(RendererType::NONE){}
	virtual ~Renderer() {}
	virtual SDL_Window* CreateWindow(std::string name_, int width_, int height_) = 0;
	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void Render() = 0;
	RendererType getRendererType() { return rendererType; }
	void setRendererType(RendererType rendererType_) { rendererType = rendererType_; }
	//////////
	/*virtual uint64_t LoadModel(std::string modelPath) = 0;
	virtual uint64_t LoadTexture2D(std::string texturePath) = 0;
	virtual uint64_t CreateShader(std::string vertPath, std::string fragPath) = 0;*/ /* , std::string tesPath, std::string tesControlPath, std::string geometryPath) = 0;*/


protected:
	Renderer *renderer;
	RendererType rendererType;
};

#endif