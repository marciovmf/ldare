#ifndef _LDK_RENDERER_H_
#define _LDK_RENDERER_H_

namespace ldk
{
	namespace render
	{
		void updateRenderer(float deltaTime);
		void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight, uint32 virtualWidth, uint32 virtualHeight);
		void setViewport(uint32 x, uint32 y, uint32 width, uint32 height);
		Shader createShaderProgram(const char8* vertex, const char8* fragment);
		Shader loadShader(const char8* vertex, const char8* fragment);
		ldk::Texture loadTexture(const char8* bitmapFile);
		ldk::Material loadMaterial(const char8* vertex, const char8* fragment, const char8* textureFile);

	}
}
#endif// _LDK_RENDERER_H_
