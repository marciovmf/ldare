#ifndef __LDARE_RENDERER__
#define __LDARE_RENDERER__

#ifdef ERROR_INVALID_HANDLE
#undef ERROR_INVALID_HANDLE
#define ERROR_INVALID_HANDLE -1
#endif

#include <ldare/ldare.h>

namespace ldare
{
	namespace renderer
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
		struct NativeResourceId
		{
			int32 i32Value;
			int64 i64Value;
		};

		struct VertexBufferResource
		{
			void* data; 											// pointer to buffer data
			size_t size; 											// size of data buffer
			void* layout; 										// pointer to buffer layout
			int32 layoutAttribCount; 					// number of buffer layout attributes
			NativeResourceId resourceId; 			// native render API resource id
			int8 bound; 											// whether or not buffer is bound to the GPU
		};

		//---------------------------------------------------------------------------
		// A structure for storing renderer resources
		//---------------------------------------------------------------------------
		struct RendererResources
		{
			int32 numVertexBuffers; 						// number of vertex buffer available
			int32 maxVertexBuffers; 						// 
		//	int32 numIndexBuffers; 						// number of index buffers available
		//	int32 maxIndexBuffers; 						//
		//	int32 numElementBuffers; 					// number of element buffers available
		//	int32 maxElementBuffers; 					//
			VertexBufferResource* vertexBufferList; 	//list of vertex buffer resources
		};

		// Supported rendering API
		enum RenderingApi
		{
			CORE_PROFILE_OPEN_GL 					// Yep, thats all we are gonna have for now!
		};

		//---------------------------------------------------------------------------
		// Initialize the renderer
		//---------------------------------------------------------------------------
		int32 initRenderer(RenderingApi api, RendererResources* resources);
		
		//---------------------------------------------------------------------------
		// Create a vertex buffer resource
		//---------------------------------------------------------------------------
		RESHANDLE createVertexBuffer(void* data, size_t size,
				VertexAttributeLayout* layout,	int32 numAttributes);

		//---------------------------------------------------------------------------
		// Delete a buffer resource
		//---------------------------------------------------------------------------
		int32 deleteVertexBuffer(RESHANDLE);

	} // namespace renderer
} // namespace ldare

#endif 		// __LDARE_RENDERER__
