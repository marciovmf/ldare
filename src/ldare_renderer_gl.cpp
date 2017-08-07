/**
 * win32_renderer_gl.h
 * Win32 implementation for ldare platform functions
 */

namespace ldare 
{
	namespace render
	{
		static ldare::memory::Heap vertexBufferHeap;

		// Checks and logs OpenGL error if any. Returns non zero if no errors was found
		static int32 checkNoGlError()
		{
			const char* error = "UNKNOWN ERROR CODE";
			GLenum err (glGetError());
			int32 success = 1;
			while(err!=GL_NO_ERROR) 
			{
				switch(err)
				{
					case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
					case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
					case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
					case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
					case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
				}
				success=0;
				LogError(error);
				err=glGetError();
			}

			return success;
		}

		//---------------------------------------------------------------------------
		// Initializes this renderer implementation
		//---------------------------------------------------------------------------
		void initRenderer(void* renderSpecific)
		{
			UNUSED_PARAMETER(renderSpecific);

			// initialize a heap for vertex buffer resources
			size_t defaultHeapSize = MEGABYTE(1);
			vertexBufferHeap.memorySize = defaultHeapSize;
			vertexBufferHeap.freeMemList =
				(ldare::memory::HeapAllocationHeader*)ldare::platform::memoryAlloc(defaultHeapSize);
			vertexBufferHeap.objectSize = sizeof(VertexBufferResource);
		}

		//---------------------------------------------------------------------------
		// Create a vertex buffer resource
		//---------------------------------------------------------------------------
		ldare::LDHANDLE createVertexBuffer(VertexBufferAccess access)
		{
			GLuint nativeResourceId;
			glGenBuffers(1, &nativeResourceId);

			if(!checkNoGlError())
			{
				LogError("Could not allocate OpenGL buffer");
				return ldare::LDHANDLE{ldare::ResourceType::INVALID, 0};
			}

			// set up buffer resource information
			VertexBufferResource* resource =
				(VertexBufferResource*)ldare::memory::getMemory(&vertexBufferHeap);
			resource->nativeResourceId = nativeResourceId;
			resource->bufferAccess = access;
			resource->bound=0;
			resource->layout=0;
			resource->data=0;
			resource->layoutAttribCount=0;

			// Set up the handle
			ldare::LDHANDLE handle;
			handle.type = ldare::ResourceType::VERTEX_BUFFER;
			handle.data = resource;
			return handle;
		}


		void bindBuffer(ldare::LDHANDLE handle)
		{
			ASSERT(handle.type != ldare::ResourceType::INVALID);
			
			switch (handle.type)
			{
				case ResourceType::VERTEX_BUFFER:
					{
						render::VertexBufferResource* vertexBuffer = 
							(render::VertexBufferResource*) handle.data;
						//TODO: implement streaming buffer binding
						//TODO: implement instance buffer binding
						GLenum glBufferTarget = render::VertexBufferAccess::STATIC;
						glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->nativeResourceId);

						int32 lastAttributeIndex = -1;
						// specify buffer layout
						for(int32 i=0; i < vertexBuffer->layoutAttribCount; i++)
						{
							render::VertexAttributeLayout layout = vertexBuffer->layout[i];
							glVertexAttribPointer(i,
									layout.count,
									layout.type,
									GL_FALSE,
									layout.stride,
									(void*)layout.startOffset);

							if ( lastAttributeIndex != i)
							{
								lastAttributeIndex = i;
								glEnableVertexAttributeArray(lastAttributeIndex);
							}
						}
					}	
					break;

				default:
					break;
			}

			checkNoGlError();
		}

		//---------------------------------------------------------------------------
		// Delete a buffer resource
		//---------------------------------------------------------------------------
		void deleteVertexBuffer(ldare::LDHANDLE handle)
		{
			ASSERT(handle.type != ldare::ResourceType::INVALID);
			ASSERT(handle.type == ldare::ResourceType::VERTEX_BUFFER);

			auto nativeResource =((VertexBufferResource*) handle.data)->nativeResourceId;
			glDeleteBuffers(1, (GLuint*)&nativeResource);

			if(!checkNoGlError())
			{
				LogError("Could not allocate OpenGL buffer");
				return;
			}

			ldare::memory::freeMemory(handle.data);
		}

	} // namespace renderer
} // namespace ldare
