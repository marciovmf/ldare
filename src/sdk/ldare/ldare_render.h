#ifndef __LDARE_RENDER__
#define __LDARE_RENDER__

namespace ldare
{
	namespace render
	{
		typedef uint32 Shader;

		struct Sprite
		{
			Vec3 position;
			Vec3 color;
			float width;
			float height;

		};

		struct SpriteVertexData
		{
			Vec3 position;
			Vec3 color;
		};

		Shader loadShader(const char* vertex, const char* fragment);
		int32 initSpriteBatch();
		void begin();
		void submit(uint32 shader, const Sprite& sprite);
		void end();
		void flush();
		
	} // namespace renderer
} // namespace ldare

#endif 		// __LDARE_RENDER__
