#ifndef __LDK_RENDER__
#define __LDK_RENDER__

namespace ldk
{
	struct Sprite
	{
		Vec3 position;
		Vec4 color;
		float width;
		float height;
		float angle;
		Rectangle srcRect;
	};

	namespace render
	{
		//
		// Describes attributes of a GPU buffer layout
		//
		struct GpuBufferLayout
		{
			enum Type{ INT8, UINT8, INT16, UINT16, FLOAT32 };
			enum Size{ X1=1, X2, X3, X4};

			uint32 index;  
			Type type;
			Size size;
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

		LDK_API	Shader createShaderProgram(const char* vertex, const char* fragment);

		LDK_API	Shader loadShader(const char* vertexSource, const char* fragmentSource);

		LDK_API Shader loadShaderFromFile(const char* vertexFile, const char* fragmentFile);

		LDK_API	Texture loadTexture(const char* bitmapFile);

		LDK_API	Material loadMaterial(const char* materialFile);

		LDK_API void unloadMaterial(Material* material);

		// Sprite batch api
		LDK_API int32 spriteBatchInit();
		LDK_API void spriteBatchBegin(const ldk::Material& material);
		LDK_API void spriteBatchSubmit(const ldk::Sprite& sprite);
		LDK_API Vec2 spriteBatchText(Vec3& position, float scale, Vec4& color, const char* text);
		LDK_API void spriteBatchFlush();
		LDK_API void spriteBatchEnd();
		LDK_API void spriteBatchSetFont(const ldk::FontAsset& font);

		//
		LDK_API void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight, uint32 virtualWidth, uint32 virtualHeight);
		LDK_API void setViewport(uint32 x, uint32 y, uint32 width, uint32 height);
		LDK_API void updateRenderer(float deltaTime);
	}
}


#endif 		// __LDK_RENDER__
