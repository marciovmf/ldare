#ifndef _LDK_GPUBUFFER_H_	
#define _LDK_GPUBUFFER_H_	

//
// The renderer namespace is for the renderer backend only. 
// It will not be exposed to the game layer.
//
namespace ldk
{
	namespace renderer
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

				Type type;
				uint32 id; 
				uint32 target;
				uint32 usage;
		};

		//
		// Creates a buffer with the given size and layout
		// return a GPU buffer identifier
		//
		renderer::GpuBuffer createBuffer(renderer::GpuBuffer::Type, 
				size_t size, 
				const renderer::GpuBufferLayout* layout, 
				uint32 layoutCount, 
				void* data = nullptr);

		//
		// Copies data to a GPU buffer, partially overwitting its content
		//
		void setBufferData(const renderer::GpuBuffer& buffer, 
				void* data, 
				size_t dataSize, 
				uint32 offset);

		//
		// Copies data to a GPU buffer, completely overwitting its content
		//
		void setBufferData(const renderer::GpuBuffer& buffer, 
				void* data, 
				size_t dataSize);

		//
		// Binds the buffer to the GPU so it can be used
		//
		void bindBuffer(const renderer::GpuBuffer& buffer);

		//
		// Unbinds the buffer from the GPU
		//
		void unbindBuffer(const renderer::GpuBuffer& buffer);

		//
		// Deletes a GPU buffer and release its GPU resources 
		//
		void deleteBuffer(renderer::GpuBuffer& buffer);

	}
}
//	using namespace ldare::renderer;
//	GpuBufferLayout layout[] = {
//		{0, GpuBufferLayout::Type::FLOAT32, GpuBufferLayout::Size::X4, 0, 0 },
//		{1, GpuBufferLayout::Type::FLOAT32, GpuBufferLayout::Size::X4, 0, 0 },
//		{2, GpuBufferLayout::Type::FLOAT32, GpuBufferLayout::Size::X4, 0, 0 }};


#endif // _LDK_GPUBUFFER_H_	
