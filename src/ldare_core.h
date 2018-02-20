#ifndef __LDARE_CORE__
#define __LDARE_CORE__

namespace ldare
{
	struct Input;

	namespace app
	{
#define OPENGL_3_2 1

		struct Window;

		bool init(uint32 renderApi);
		Window* createWindow(uint32 width,	uint32 height, const char* title);
		void swapBuffer(Window& window); 
		bool shouldClose(Window& window);
		void pollEvents(Window& window, ldare::Input& input);
		void terminate();
	}
}

#endif // __LDARE_CORE__
