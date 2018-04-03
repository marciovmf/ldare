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
		void setClearColor(Vec4 color);
		void clear();
		void draw();
		void drawIndexed();
		//TODO: Support instanced rendering
		//void DrawInstanced();
	}
}

#endif // _LDK_RENDERER_H_
