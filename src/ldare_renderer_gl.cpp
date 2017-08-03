#include "../ldare_renderer.h"
#include "../ldare_gl.h"

namespace ldare 
{
	namespace renderer
	{
		static RendererResources* resources;

		// Checks and logs OpenGL error if any. Returns non zero if no errors was found
		static GLenum checkNoGlError()
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

		//TODO: Do native openGL initialization here

		//---------------------------------------------------------------------------
		// Initializes this renderer implementation
		//---------------------------------------------------------------------------
		int32 initRenderer(RenderingApi api, RendererResources* rendererResources)
		{
			resources = rendererResources;
			return 0;
		}

		//---------------------------------------------------------------------------
		// Create a vertex buffer resource
		//---------------------------------------------------------------------------
			RESHANDLE createVertexBuffer(void* data, size_t size,
				VertexAttributeLayout* layout,	int32 numAttributes)
		{
			if (resources->numVertexBuffers >= resources->maxVertexBuffers)
			{
				//TODO: Recicle freed buffers
				LogError("Not enough vertex buffer resources to allocate");
				return ERROR_INVALID_HANDLE;
			}

			NativeResourceId resourceId;
			glGenBuffers(1, (GLuint*) &resourceId.i32Value);

			if ( ! resourceId.i32Value || !checkNoGlError())
			{
				return ERROR_INVALID_HANDLE;
			}

			RESHANDLE handle = resources->numVertexBuffers++;
			VertexBufferResource &buffer = resources->vertexBufferList[handle];

			buffer.data = data;
			buffer.layout = layout;
			buffer.layoutAttribCount = numAttributes;
			buffer.bound = 0;
			buffer.resourceId =	resourceId;

			return handle;
		}


		//---------------------------------------------------------------------------
		// Delete a buffer resource
		//---------------------------------------------------------------------------
		int32 deleteVertexBuffer(RESHANDLE)
		{
			//TODO: implement vertex buffer recycling
			return 0;
		}
	} // namespace renderer
} // namespace ldare
