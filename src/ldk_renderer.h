#ifndef _LDK_RENDERER_H_
#define _LDK_RENDERER_H_

namespace ldk
{
	namespace render
	{
		LDK_API	void updateRenderer(float deltaTime);
		LDK_API	void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight, uint32 virtualWidth, uint32 virtualHeight);
		LDK_API	void setViewport(uint32 x, uint32 y, uint32 width, uint32 height);
		LDK_API	Shader createShaderProgram(const char8* vertex, const char8* fragment);
		LDK_API	Shader loadShader(const char8* vertex, const char8* fragment);
		LDK_API	ldk::Texture loadTexture(const char8* bitmapFile);
		LDK_API	ldk::Material loadMaterial(const char8* vertex, const char8* fragment, const char8* textureFile);

	}
}
#endif// _LDK_RENDERER_H_
