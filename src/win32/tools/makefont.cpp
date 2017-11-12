#include <ldare/ldare.h>
#include <windows.h>
#include <tchar.h>
#include <fstream>

using namespace std;

struct FontImportInput
{
	char* ttfFontFile;
	char* fontName;
	char* fontString;
	uint32 fontStringLen;
	uint32 maxLineWidth;
	uint32 fontSize;
};

static char bmpFileName[128];

static void createBitmapInfo(BITMAPINFO* bmpInfo, uint32 width, uint32 height)
{
	BITMAPINFO bmp = {};
	bmpInfo->bmiHeader.biSize = sizeof(bmp.bmiHeader);
	bmpInfo->bmiHeader.biWidth = width;
	bmpInfo->bmiHeader.biHeight = height;
	bmpInfo->bmiHeader.biPlanes = 1;
	bmpInfo->bmiHeader.biBitCount = 32;
	bmpInfo->bmiHeader.biCompression = BI_RGB;
}

static HDC makeFontDC(void** bmpMem, uint32 width, uint32 height)
{
	HDC dc = CreateCompatibleDC(GetDC(0));
	BITMAPINFO bmp;
	createBitmapInfo(&bmp, width, height);
	HBITMAP hBitmap = CreateDIBSection(dc, &bmp, DIB_RGB_COLORS, bmpMem, 0, 0);
	SelectObject(dc, hBitmap);
	//SetBkColor(dc, RGB(255, 0, 0));
	return dc;
}

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

static HGDIOBJ loadFontFile(HDC dc, char* fontFile,  char* fontName, uint32 fontSize)
{
	if (!AddFontResourceEx(fontFile, FR_PRIVATE, 0))
	{
		return 0;
	}

	LOGFONT logFont =
	{
		fontSize,//-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSX), fontSize),
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

static RECT calcFontBitmapSize(HDC dc, const char* fontString, uint32 maxLineWidth, uint32 spacing)
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

static bool parseArgs(uint32 argc, _TCHAR** argv, FontImportInput* input)
{
	if (argc != 6)
	{
		printf("usage:\n makefont TTF-file font-name font-size maxwidth fontstring\n");
		return 0;
	}

	input->ttfFontFile = argv[1];
	input->fontName = (char*)argv[2];
	input->fontSize = atoi(argv[3]);
	input->maxLineWidth = nextPow2(atoi(argv[4])-1);
	input->fontString = argv[5];
	input->fontStringLen = strlen(input->fontString);
	return true;
}

static void saveBitmap(HDC dc, RECT bitmapRect, const char* filename)
{
	// bitmap dimensions
	int bitmap_dx = bitmapRect.right -  bitmapRect.left;
	int bitmap_dy = bitmapRect.bottom - bitmapRect.top;

	// create file
	ofstream file(filename, ios::binary);
	if(!file) return;

	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType      = 0x4d42;
	fileHeader.bfSize      = 0;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFO bitmapInfo;
	createBitmapInfo(&bitmapInfo, bitmap_dx, bitmap_dy);

	// dibsection information
	BITMAPINFO info;
	info.bmiHeader = bitmapInfo.bmiHeader;
	int bitmapSize= bitmapInfo.bmiHeader.biBitCount/8 * (bitmap_dx * bitmap_dy);

	BYTE* memory = 0;
	HDC memDC =  CreateCompatibleDC(dc);
	HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**)&memory, 0, 0);
	SelectObject(memDC, bitmap);
	
	//BitBlt(memDC, 0, 0, bitmap_dx, bitmap_dy, dc, 0, 0, SRCERASE);
	BLENDFUNCTION blend = {};
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;

	AlphaBlend(memDC, 0, 0, bitmap_dx, bitmap_dy, dc, 0, 0, bitmap_dx, bitmap_dy, blend);

	// save dibsection data
	file.write((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
	file.write((char*)&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	file.write((const char*)memory, bitmapSize);
	file.close();

	DeleteDC(memDC);
	DeleteObject(bitmap);
}

int _tmain(int argc, _TCHAR** argv)
{
	FontImportInput input;

	if (!parseArgs(argc, argv, &input))
	{
		return 1;
	}

	uint32 fontWidth = 1024;
	uint32 fontHeight = 1024;
	uint32 dcBitmapSize = 1024 * 1024 * 4;
	void* bmpMem;

	HDC dc = makeFontDC(&bmpMem, fontWidth, fontHeight);
	memset(bmpMem, 0, dcBitmapSize);

	if (dc == 0)
	{
		LogInfo("Could not create a compatible Device context");
		return 1; 
	}

	HFONT hFont = (HFONT)loadFontFile(dc, input.ttfFontFile, input.fontName, input.fontSize);
	if (!hFont)
	{
		LogInfo("Could not load font file %s", input.ttfFontFile);
		return 1;
	}

	TEXTMETRIC fontMetrics;
	if (!GetTextMetrics(dc, &fontMetrics))
	{
		LogInfo("Could not get metrics for font file %s", input.ttfFontFile);
		return 1; 
	}

	const uint32 spacing = 2;

	RECT rect = calcFontBitmapSize(dc, input.fontString, input.maxLineWidth, spacing);
	HBITMAP hDcBitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom);
	SelectObject(dc, hDcBitmap);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, RGB(255,255,255));

	uint32 gliphX = 0;
	uint32 gliphY = 0;
	char gliph;
	char* fontStringPtr = input.fontString;

	uint32 nextLine = input.fontStringLen/2;
	for (int i=0; i < input.fontStringLen; i++)
	{
		// TODO: get proper gliph height
		SIZE gliphSize;
		gliph = *fontStringPtr;
		++fontStringPtr;

		GetTextExtentPoint32(dc, &gliph, 1, &gliphSize);

		// check if we need to break the line
		if (gliphX + gliphSize.cx >= input.maxLineWidth)
		{
			gliphX = 0;
			gliphY += gliphSize.cy + spacing;
		}

		LogInfo("Gliph '%c' (%d) {%d, %d, %d, %d}", gliph, gliph, gliphX, gliphY, gliphSize.cx, gliphSize.cy, 0);
		SetBkMode(dc, TRANSPARENT);
		TextOut(dc, gliphX, gliphY, &gliph, 1);
		gliphX += gliphSize.cx + spacing;
	}


	sprintf(bmpFileName, "%s.bmp", input.fontName);
	saveBitmap(dc,rect, bmpFileName);
	return 0;
}
