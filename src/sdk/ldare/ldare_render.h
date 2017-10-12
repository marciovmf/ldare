#ifndef __LDARE_RENDER__
#define __LDARE_RENDER__

namespace ldare
{
	typedef uint32 Shader;

	struct GlobalShaderData
	{
		Mat4 projectionMatrix;
		Mat4 baseModelMatrix;
		Vec2 time; // (deltaTime, time)
	};

	struct Sprite
	{
		Vec3 position;
		Vec3 color;
		float width;
		float height;
		Rectangle srcRect;
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

	void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight, uint32 virtualWidth, uint32 virtualHeight);
	void setViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	void updateRenderer(float deltaTime);
} // namespace ldare

#endif 		// __LDARE_RENDER__
