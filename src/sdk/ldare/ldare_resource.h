#ifndef __LDARE_MATERIAL__
#define __LDARE_MATERIAL__

namespace ldare
{
	struct Texture
	{
		uint32 id;
		uint32 width;
		uint32 height;
	};

	struct Material
	{
		ldare::Shader shader;
		Texture texture;
	};
}

#endif // __LDARE_MATERIAL__
