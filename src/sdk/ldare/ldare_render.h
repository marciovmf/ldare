#ifndef __LDARE_RENDER__
#define __LDARE_RENDER__

namespace ldare
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
		Vec2 uv;
	};


#define SPRITE_BATCH_BEGIN_FUNC(name) void name()
	typedef SPRITE_BATCH_BEGIN_FUNC(SpriteBatchBeginFunc);

#define SPRITE_BATCH_END_FUNC(name) void name()
	typedef SPRITE_BATCH_END_FUNC(SpriteBatchEndFunc);

#define SPRITE_BATCH_SUBMIT_FUNC(name) void name(const ldare::Material& material, const ldare::Sprite& sprite)
	typedef SPRITE_BATCH_SUBMIT_FUNC(SpriteBatchSubmitFunc);

#define SPRITE_BATCH_FLUSH_FUNC(name) void name()
	typedef SPRITE_BATCH_FLUSH_FUNC(SpriteBatchFlushFunc);

#define SPRITE_BATCH_LOAD_SHADER_FUNC(name) Shader name(const char8* vertex, const char8* fragment)
	typedef SPRITE_BATCH_LOAD_SHADER_FUNC(SpriteBatchLoadShaderFunc);

	struct SpriteBatchApi
	{
		SpriteBatchBeginFunc* begin;
		SpriteBatchSubmitFunc* submit;
		SpriteBatchEndFunc* end;
		SpriteBatchFlushFunc* flush;
		SpriteBatchLoadShaderFunc *loadShader;
	};

//	ldare::Texture loadTexture(const char* bitmapFile);

} // namespace ldare

#endif 		// __LDARE_RENDER__
