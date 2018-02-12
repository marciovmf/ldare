#ifndef __LDARE_RENDERER__
#define __LDARE_RENDERER__

//
// The renderer namespace is for the renderer backend only. 
// It will not be exposed to the game layer.
//
namespace ldare
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

#endif // __LDARE_RENDERER__
