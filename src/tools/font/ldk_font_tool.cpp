#include <ldk/ldk.h>
#include "../ldk_platform.h"
#include "../ldk_ttf.h"
#include "ldk_sdf.cpp"
#include <tchar.h>
#include <fstream>

using namespace std;

#define MAX_FONT_NAME_SIZE 32

struct FontImportSettings
{
  const char* ttfFontFile;
  char* fontString;
  uint32 fontStringLen;
  uint32 maxLineWidth;
  uint32 fontSize;
};

static uint32 nextPow2(uint32 value)
{
  --value;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  return ++value;
}

void writeBitmapToFile(ldk::Bitmap* bitmap, const char* fileName)
{
  FILE* fd = fopen(fileName, "wb+");

  // create file
  if(!fd) return;

  // Bitmal file header
  BITMAPFILEHEADER fileHeader;
  fileHeader.bfType      = 0x4d42;
  fileHeader.bfSize      = 0;
  fileHeader.bfReserved1 = 0;
  fileHeader.bfReserved2 = 0;
  fileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

  // Final BITMAPINFO to be written to the file
  BITMAPINFO bitmapInfo = {};
  bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
  bitmapInfo.bmiHeader.biWidth = bitmap->width;
  bitmapInfo.bmiHeader.biHeight = bitmap->height;
  bitmapInfo.bmiHeader.biPlanes = 1;
  bitmapInfo.bmiHeader.biBitCount = 32;
  bitmapInfo.bmiHeader.biCompression = BI_RGB;
 
  const size_t bmpDataSize = (bitmap->bitsPerPixel/8) * (bitmap->width * bitmap->height);

  fwrite(&fileHeader, sizeof(int8), sizeof(fileHeader), fd);
  fwrite(&bitmapInfo, sizeof(int8), sizeof(bitmapInfo), fd);
  fwrite(bitmap->pixels, sizeof(int8), bmpDataSize, fd);
  fclose(fd);
}


void computeBitmapSDF(ldk::Bitmap* bitmap)
{
  const uint32 width = bitmap->width;
  const uint32 height = bitmap->height;
  const uint32 pitch = bitmap->bitsPerPixel/8 * width;

  const Point BLACK = {0, 0};
  const Point WHITE = {9999, 9999};

  Grid grid1(width, height);
  Grid grid2(width, height);

  // generate the grids
	for(int32 y = height-1; y >= 0; y--)
	{
		for (int32 x=0; x<width; x++)
		{
			uint32 *src = ((uint32*)((uint8*)bitmap->pixels + y*pitch)) + x;
			uint8 color = (*src & 0x00FF0000) >> 16;
			
			// Points inside get marked with a dx/dy of zero.
			// Points outside get marked with an infinitely large distance.
			if (color < 128)
			{
				put(grid1, x, y, BLACK);
				put(grid2, x, y, WHITE);
			} 
      else 
      {
				put(grid2, x, y, BLACK);
				put(grid1, x, y, WHITE);
			}
		}
	}

	GenerateSDF(grid1);
	GenerateSDF(grid2);

  // apply the result to the bitmap
	for(int32 y = height - 1; y >= 0; y--)
	{
		for (int32 x=0; x < width; x++)
		{
			// Calculate the actual distance from the dx/dy
			int32 dist1 = (int32)(sqrt((double)get(grid1, x, y).euclideanDist()));
			int32 dist2 = (int32)(sqrt((double)get(grid2, x, y).euclideanDist()));
			int32 dist = dist1 - dist2;

			// Clamp and scale it, just for display purposes.
			int32 color = dist*3 + 128;
			if (color < 0) color = 0;
			if (color > 255 ) color = 255;
			uint32 *dest = ((uint32*)((uint8*)bitmap->pixels + y*pitch)) + x;
			*dest = color << 24 | color << 16 | color << 8 | color;
		}
	}
}

#ifdef _LDK_WINDOWS_
  #include "ldk_font_tool_win32.cpp"
#else
  #pragma "Sorry, This was not implemented for this platform";
#endif

int main(int argc, _TCHAR** argv)
{
  if (argc != 4)
  {
    printf("usage:\n makefont TTF-file font-size maxwidth\n");
    return 0;
  }

  const char* ttfFontFile = argv[1];
  const uint32 fontSize = atoi(argv[2]);

  Context context = initContext();
  ldk::FontMetrics fontMetrics = getFontMetrics(ttfFontFile, context);

  // Setup the import settings
  FontImportSettings settings;
  settings.ttfFontFile    = ttfFontFile;
  settings.fontSize       = fontSize;
  settings.maxLineWidth   = nextPow2(atoi(argv[3])-1);
  settings.fontStringLen  = fontMetrics.lastCodePoint - fontMetrics.firstCodePoint;
  int8* fontBuffer        = new int8[settings.fontStringLen];
  settings.fontString     = (char*)fontBuffer;

  // Create the font string
  for (uint32 i = 0; i < settings.fontStringLen; i++ )
  {
    uint8 codePoint = MAX(1, fontMetrics.firstCodePoint + i);

    if (codePoint < 32)
      codePoint = fontMetrics.defaultCodePoint;
    else if (codePoint == 127)
      codePoint = fontMetrics.defaultCodePoint;

    fontBuffer[i] = codePoint;
  }
  
  ldk::Bitmap* bitmap = importFontFromTTF(settings, fontMetrics, context);
  computeBitmapSDF(bitmap);
  writeBitmapToFile(bitmap, "test.bmp");
  destroyContext(context);
  delete bitmap;

  delete fontBuffer;
  LogInfo("Success");
  return 0;
}

