#ifndef _LDK_RENDERER_H_
#define _LDK_RENDERER_H_

//
// The renderer namespace is for the renderer backend only. 
// It will not be exposed to the game layer.
//
namespace ldk
{
	namespace renderer
	{	
		struct Material;
		struct GpuBuffer;

		void setClearColor(Vec4 color);
		void clear();

		void drawIndexed(Material* material, GpuBuffer* vertexBuffer, GpuBuffer* indexBuffer, uint32 indexCount, uint32 start);
	}
}

#endif // _LDK_RENDERER_H_
