#ifndef __LDK_RENDER__
#define __LDK_RENDER__

namespace ldk
{
	namespace render
	{
		//
		// Describes attributes of a GPU buffer layout
		//
		struct GpuBufferLayout
		{
			enum Type{ INT8, UINT8, INT16, UINT16, FLOAT32 };
			enum Size{ X1=1, X2, X3, X4};

			Type type;
			Size size;
			uint32 index;  
			uint32 stride;
			uint32 start;
		};

		//
		// A GPU buffer that can be filled with data for rendering.
		//
		struct GpuBuffer
		{
				enum Type{ VERTEX, VERTEX_DYNAMIC, VERTEX_STREAM, INDEX, UNIFORM };

				Type		type;
				uint32	id; 
				uint32	target;
				uint32	usage;
		};

		//
		// Creates a buffer with the given size and layout
		// return a GPU buffer identifier
		//
		LDK_API render::GpuBuffer createBuffer(render::GpuBuffer::Type, 
				size_t size, 
				const render::GpuBufferLayout* layout, 
				uint32 layoutCount, 
				void* data = nullptr);

		//
		// Copies data to a GPU buffer, partially overwitting its content
		//
		LDK_API void setBufferSubData(const render::GpuBuffer& buffer, 
				void* data, 
				size_t dataSize, 
				uint32 offset);

		//
		// Copies data to a GPU buffer, completely overwitting its content
		//
		LDK_API void setBufferData(const render::GpuBuffer& buffer, 
				void* data, 
				size_t dataSize);

		//
		// Binds the buffer to the GPU so it can be used
		//
		LDK_API void bindBuffer(const render::GpuBuffer& buffer);

		//
		// Unbinds the buffer from the GPU
		//
		LDK_API void unbindBuffer(const render::GpuBuffer& buffer);

		//
		// Deletes a GPU buffer and release its GPU resources 
		//
		LDK_API void deleteBuffer(render::GpuBuffer& buffer);

		LDK_API	void updateRenderer(float deltaTime);

		LDK_API	void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight,
				uint32 virtualWidth, uint32 virtualHeight);

		LDK_API	void setViewport(uint32 x, uint32 y, uint32 width, uint32 height);

		LDK_API	Shader createShaderProgram(const char8* vertex, const char8* fragment);

		LDK_API	Shader loadShader(const char8* vertex, const char8* fragment);

		LDK_API	Texture loadTexture(const char8* bitmapFile);

		LDK_API	Material loadMaterial(const char8* vertex, const char8* fragment, 
				const char8* textureFile);
	}
}

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

#endif 		// __LDK_RENDER__
