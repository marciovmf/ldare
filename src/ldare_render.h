#ifndef __LDARE_RENDER__
#define __LDARE_RENDER__

namespace ldare
{
	namespace render
	{
		enum PrimitiveType
		{
			TRIANGLE,
			TRIANGLE_STRIP,
		};

		//---------------------------------------------------------------------------
		// Generic buffer attribute
		//---------------------------------------------------------------------------
		struct VertexAttributeLayout
		{
			enum VertexAttributeDataType
			{
				INT16,
				UINT16,
				INT32,
				UINT32,
				FLOAT32,
				DOUBLE64
			} type;
			int32 count;
			int32 stride;
			int32 startOffset;
		};

		//---------------------------------------------------------------------------
		// A buffer for vertex related data
		//---------------------------------------------------------------------------
		enum VertexBufferAccess
		{
			STATIC 														//TODO: Implement streaming buffer access
		};
		
		struct VertexBufferResource
		{
			void* data; 											// pointer to buffer data
			VertexAttributeLayout* layout;		// pointer to buffer layout
			size_t size; 											// size of data buffer
			int32 layoutAttribCount; 					// number of buffer layout attributes
			VertexBufferAccess bufferAccess; 	// how is this bufffer gonna be accessed	
			int32 nativeResourceId;
			int8 bound; 											// whether or not buffer is bound to the GPU
		};

		struct RendererResources
		{
			int vertexBufferCount; 								// how many vertex buffer resources are created
			VertexBufferResource* vertexBuffers; 	// pointer to all vertex buffer resources
		};
		
		//---------------------------------------------------------------------------
		// Initialize the renderer
		//---------------------------------------------------------------------------
		void initRenderer(void* renderSpecific);
		
		//---------------------------------------------------------------------------
		// Create a vertex buffer resource
		//---------------------------------------------------------------------------
		ldare::LDHANDLE createVertexBuffer(VertexBufferAccess access);

		void bindBuffer(ldare::LDHANDLE buffer);

		//---------------------------------------------------------------------------
		// Delete a buffer resource
		//---------------------------------------------------------------------------
		void deleteVertexBuffer(ldare::LDHANDLE);

	} // namespace renderer
} // namespace ldare

#endif 		// __LDARE_RENDER__
