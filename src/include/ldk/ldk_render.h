#ifndef __LDK_RENDER__
#define __LDK_RENDER__
namespace ldk
{


#if 0
#define SPRITE_BATCH_BEGIN_FUNC(name) void name(const ldk::Material& material)
	typedef SPRITE_BATCH_BEGIN_FUNC(SpriteBatchBeginFunc);

#define SPRITE_BATCH_END_FUNC(name) void name()
	typedef SPRITE_BATCH_END_FUNC(SpriteBatchEndFunc);

#define SPRITE_BATCH_SUBMIT_FUNC(name) void name(const ldk::Sprite& sprite)
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

#define TEXT_BATCH_BEGIN_FUNC(name) void name(const ldk::FontAsset&, const ldk::Material& material)
	typedef TEXT_BATCH_BEGIN_FUNC(TextBatchBeginFunc);

#define TEXT_BATCH_END_FUNC(name) void name()
	typedef TEXT_BATCH_END_FUNC(TextBatchEndFunc);

#define TEXT_BATCH_DRAW_TEXT_FUNC(name) Vec2 name(Vec3& position, float scale, Vec4& color, const char* text)
	typedef TEXT_BATCH_DRAW_TEXT_FUNC(TextBatchDrawTextFunc);

#define TEXT_BATCH_FLUSH_FUNC(name) void name()
	typedef TEXT_BATCH_FLUSH_FUNC(TextBatchFlushFunc);

	//TODO: This is temporary solution just for LundumDare. Remove this when we hav draw call sorting.
	struct TextBatchApi
	{
		TextBatchBeginFunc* begin;
		TextBatchDrawTextFunc* drawText;
		TextBatchEndFunc* end;
		TextBatchFlushFunc* flush;
	};

	void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight, uint32 virtualWidth, uint32 virtualHeight);
	void setViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	void updateRenderer(float deltaTime);
#endif // 0

} // namespace ldk
#endif 		// __LDK_RENDER__
