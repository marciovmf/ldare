#ifndef __LDARE_RENDER__
#define __LDARE_RENDER__

namespace ldare
{
	struct GameApi;

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

#define SPRITE_BATCH_BEGIN_FUNC(name) void name()
		typedef SPRITE_BATCH_BEGIN_FUNC(SpriteBatchBeginFunc);

#define SPRITE_BATCH_END_FUNC(name) void name()
		typedef SPRITE_BATCH_END_FUNC(SpriteBatchEndFunc);

#define SPRITE_BATCH_SUBMIT_FUNC(name) void name(uint32 shader, const Sprite& sprite)
		typedef SPRITE_BATCH_SUBMIT_FUNC(SpriteBatchSubmitFunc);

#define SPRITE_BATCH_FLUSH_FUNC(name) void name()
		typedef SPRITE_BATCH_FLUSH_FUNC(SpriteBatchFlushFunc);

#define SPRITE_BATCH_LOAD_SHADER_FUNC(name) Shader name(const char* vertex, const char* fragment)
		typedef SPRITE_BATCH_LOAD_SHADER_FUNC(SpriteBatchLoadShaderFunc);

		struct SpriteBatchApi
		{
			SpriteBatchBeginFunc* begin;
			SpriteBatchSubmitFunc* submit;
			SpriteBatchEndFunc* end;
			SpriteBatchFlushFunc* flush;
			SpriteBatchLoadShaderFunc *loadShader;
		};

	} // namespace renderer
} // namespace ldare

#endif 		// __LDARE_RENDER__
