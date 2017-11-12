#include <ldare/ldare.h>
#include <windows.h>
#include <tchar.h>
#include <fstream>

using namespace std;

HDC makeFontDC(void** bmpMem, uint32 width, uint32 height)
{
	HDC dc = CreateCompatibleDC(GetDC(0));
	BITMAPINFO bmp = {};
	bmp.bmiHeader.biSize = sizeof(bmp.bmiHeader);
	bmp.bmiHeader.biWidth = width;
	bmp.bmiHeader.biHeight = height;
	bmp.bmiHeader.biPlanes = 1;
	bmp.bmiHeader.biBitCount = 32;
	bmp.bmiHeader.biCompression = BI_RGB;
	HBITMAP hBitmap = CreateDIBSection(dc, &bmp, DIB_RGB_COLORS, bmpMem, 0, 0);
	SelectObject(dc, hBitmap);
	SetBkColor(dc, RGB(0, 0, 0));
	return dc;
}

void saveBitmap(HDC dc, RECT bitmapRect, const char* filename)
{
	// bitmap dimensions
	int bitmap_dx = bitmapRect.right -  bitmapRect.left;
	int bitmap_dy = bitmapRect.bottom - bitmapRect.top;

	// create file
	ofstream file(filename, ios::binary);
	if(!file) return;

	// save bitmap file headers
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fileHeader.bfType      = 0x4d42;
	fileHeader.bfSize      = 0;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	infoHeader.biSize          = sizeof(infoHeader);
	infoHeader.biWidth         = bitmap_dx;
	infoHeader.biHeight        = bitmap_dy;
	infoHeader.biPlanes        = 1;
	infoHeader.biBitCount      = 24;
	infoHeader.biCompression   = BI_RGB;
	infoHeader.biSizeImage     = 0;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed       = 0;
	infoHeader.biClrImportant  = 0;

	file.write((char*)&fileHeader, sizeof(fileHeader));
	file.write((char*)&infoHeader, sizeof(infoHeader));

	// dibsection information
	BITMAPINFO info;
	info.bmiHeader = infoHeader; 

	BYTE* memory = 0;
	HDC memDC =  CreateCompatibleDC(dc);
	HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**)&memory, 0, 0);
	SelectObject(memDC, bitmap);
	BitBlt(memDC, 0, 0, bitmap_dx, bitmap_dy, dc, 0, 0, SRCCOPY);
	DeleteDC(memDC);

	// save dibsection data
	int bytes = (((24*bitmap_dx + 31) & (~31))/8)*bitmap_dy;
	file.write((const char*)memory, bytes);

	DeleteObject(bitmap);
}

uint32 nextPow2(uint32 value)
{
 --value;
 value |= value >> 1;
 value |= value >> 2;
 value |= value >> 4;
 value |= value >> 8;
 value |= value >> 16;
 return ++value;
}

HGDIOBJ loadFontFile(HDC dc, char* fontFile,  char* fontName, uint32 fontSize)
{
	if (!AddFontResourceEx(fontFile, FR_PRIVATE, 0))
	{
		return 0;
	}

	LOGFONT logFont =
	{
		-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSX), 72),
		0,
		0,
		0,
		FW_NORMAL,
		FALSE, // italic
		FALSE, // underline
		FALSE, // strikeout
		DEFAULT_CHARSET,
		OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH
	};

	uint32 fontNameLen = strlen(fontName);
	if (fontNameLen > LF_FACESIZE) fontNameLen = LF_FACESIZE;

	for (int i=0; i < fontNameLen; i++)
	{
   logFont.lfFaceName[i] = fontName[i];
	}

	HFONT hFont = CreateFontIndirect(&logFont);
	if (!hFont)
	{
		LogError("Could not create logic font");
		return 0;
	}

	return SelectObject(dc, hFont);
}

RECT calcFontBitmapSize(HDC dc, const char* fontString, uint32 maxLineWidth, uint32 spacing)
{
	RECT rect = {};
	SIZE fontStringSize;
	uint32 fontStringLen = strlen(fontString);
	GetTextExtentPoint32(dc, fontString, fontStringLen, &fontStringSize);
	uint32 fullLineSize = fontStringSize.cx + fontStringLen * spacing;
	uint32 numLines =  fullLineSize / maxLineWidth + 1;
	rect.right = nextPow2(fullLineSize / numLines);
	rect.bottom = nextPow2(spacing + numLines * fontStringSize.cy);
	return rect;
}

char bmpFileName[128];

int _tmain(int argc, _TCHAR** argv)
{
	if (argc != 5)
	{
		LogInfo("usage:\n makefont TTF-file font-size font-name maxwidth");
		return 0;
	}

	char* fontFile = argv[1];
	uint32 fontSize = atoi(argv[2]);
	_TCHAR* fontName = (char*)argv[3];
	uint32 maxLineWidth = nextPow2(atoi(argv[4])-1);
	HDC dc;
  const char* fontString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-:,[{()}]";
	int32 fontStringLen = strlen(fontString);
  HFONT hFont;

	uint32 fontWidth = 1024;
	uint32 fontHeight = 1024;
	uint32 dcBitmapSize = 1024 * 1024 * sizeof(uint32);
  void* bmpMem = malloc(dcBitmapSize);
	memset(bmpMem, 0, dcBitmapSize);

	dc = makeFontDC(&bmpMem, fontWidth, fontHeight);

	if (dc == 0)
	{
		LogInfo("Could not create a compatible Device context");
		return 1; 
	}

	hFont = (HFONT)loadFontFile(dc, fontFile, fontName, fontSize);
	if (!hFont)
	{
		LogInfo("Could not load font file %s", fontFile);
		return 1;
	}

	TEXTMETRIC fontMetrics;
	if (!GetTextMetrics(dc, &fontMetrics))
	{
		LogInfo("Could not get metrics for font file %s", fontFile);
		return 1; 
	}

	const uint32 spacing = 2;

	RECT rect = calcFontBitmapSize(dc, fontString, maxLineWidth, spacing);
	HBITMAP hDcBitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom);
	SelectObject(dc, hDcBitmap);
	SetTextColor(dc, RGB(255,255,255));
	SetBkColor(dc,RGB(0,0,0));

	uint32 gliphX = 0;
	uint32 gliphY = 0;
	char gliph;
  
  uint32 nextLine = fontStringLen/2;
	for (int i=0; i < fontStringLen; i++)
	{
		// TODO: get proper gliph height
		SIZE gliphSize;
		gliph = *fontString;
		++fontString;

		GetTextExtentPoint32(dc, &gliph, 1, &gliphSize);

		// check if we need to break the line
		if (gliphX + gliphSize.cx >= maxLineWidth)
		{
			gliphX = 0;
			gliphY += gliphSize.cy + spacing;
		}
		
		LogInfo("Gliph '%c' (%d) {%d, %d, %d, %d}", gliph, gliph, gliphX, gliphY, gliphSize.cx, gliphSize.cy, 0);
		TextOut(dc, gliphX, gliphY, &gliph, 1);
		gliphX += gliphSize.cx + spacing;
	}


  sprintf(bmpFileName, "%s.bmp", fontName);
	saveBitmap(dc,rect, bmpFileName);
	return 0;
}
