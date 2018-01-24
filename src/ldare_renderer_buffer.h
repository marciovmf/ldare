#ifndef __LDARE_RENDERER_BUFFER__	
#define __LDARE_RENDERER_BUFFER__	

//
// The renderer namespace is for the rendere backend only. 
// It will not be exposed to the game layer.
//
namespace ldare{

	namespace renderer
	{	
		//
		// Describes attributes of a GPU buffer layout
		//
		struct BufferLayout
		{
			enum Type{ INT8, UINT8, INT16, UINT16, FLOAT32 };
			enum Size{ X1=1, X2, X3, X4};

			uint32 index;  
			Type type;
			Size size;
			uint32 start;
			uint32 stride;
		};

		//
		// A GPU buffer that can be filled with data for rendering.
		//
		struct Buffer
		{
			enum Type{ VERTEX, VERTEX_DYNAMIC, VERTEX_STREAM, INDEX, UNIFORM };
			struct
			{
				uint32 id; 
				uint32 target;
				uint32 usage;
			} GL;
		};

		//
		// Creates a buffer with the given size and layout
		// return a GPU buffer identifier
		//
		renderer::Buffer createBuffer(renderer::Buffer::Type, size_t size, const renderer::BufferLayout* layout, uint32 layoutCount);

		//
		// Copies data to a GPU buffer, partially overwitting its content
		//
		void setBufferData(const renderer::Buffer& buffer, void* data, size_t dataSize, uint32 offset);

		//
		// Copies data to a GPU buffer, completely overwitting its content
		//
		void setBufferData(const renderer::Buffer& buffer, void* data, size_t dataSize);

		//
		// Binds the buffer to the GPU so it can be used
		//
		void bindBuffer(const renderer::Buffer& buffer);

		//
		// Unbinds the buffer from the GPU
		//
		void unbindBuffer(const renderer::Buffer& buffer);

		//
		// Deletes a GPU buffer and release its GPU resources 
		//
		void deleteBuffer(renderer::Buffer& buffer);

	}
}
//	using namespace ldare::renderer;
//	BufferLayout layout[] = {
//		{0, BufferLayout::Type::FLOAT32, BufferLayout::Size::X4, 0, 0 },
//		{1, BufferLayout::Type::FLOAT32, BufferLayout::Size::X4, 0, 0 },
//		{2, BufferLayout::Type::FLOAT32, BufferLayout::Size::X4, 0, 0 }};


#endif // __LDARE_RENDERER_BUFFER__	
