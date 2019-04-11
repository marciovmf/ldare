#include <ldk/ldk.h>
#include <ldk/ldk_asset.h>
#include "../../ldk_ttf.h"
#include <windows.h>
#include <tchar.h>
#include <fstream>

using namespace std;

#define MAX_FONT_NAME_SIZE 32

struct FontImportInput
{
	char* ttfFontFile;
	char* fontString;
	uint32 fontStringLen;
	uint32 maxLineWidth;
	uint32 fontSize;
};

uint32 getFontName(const char* ttfFile, char* fontNameBuffer, uint32 fontNameBufferSize)
{
	uint32 bufferSize;
	int8* data;
	DWORD numBytesRead=0;
	HANDLE hFile = CreateFile(ttfFile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	// Look for HEAD table
	if (hFile == INVALID_HANDLE_VALUE)
	{
		LogError("Could not load file %s", ttfFile);
		return false;
	}

	bufferSize = GetFileSize(hFile, NULL);
	data = new int8[bufferSize];

	ReadFile(hFile, (void*)data, bufferSize, &numBytesRead, NULL);
	DWORD lastError = GetLastError();

	const TTFNameTableHeader *nameTable = (TTFNameTableHeader*) 
		getTTFTablePtr((const uint8*)data, TTF_NAME_TABLE);

	uint32 numCharacterCopied = getTTFString(nameTable, fontNameBuffer, fontNameBufferSize, 
			TTF_NAME_FONT_FAMILY_ID);

	CloseHandle(hFile);
	delete data;
	return numCharacterCopied;
}

static HDC makeFontDC(void** bmpMem, uint32 width, uint32 height)
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

static HGDIOBJ installSystemFontFromTTF(HDC dc, const char* fontFile,  const char* fontName, uint32 fontSize)
{
	if (!AddFontResourceEx(fontFile, FR_PRIVATE, 0))
	{
		return 0;
	}

	LOGFONT logFont =
	{
		//-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSX), 72),
		//40,
		-fontSize,
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

static RECT calcFontBitmapSize(HDC dc, const char* fontString, uint32 maxLineWidth, 
		uint32 spacing)
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

static void saveBitmap(HDC dc, RECT bitmapRect, const char* filename)
{
	// bitmap dimensions
	uint32 bitmap_width = bitmapRect.right -  bitmapRect.left;
	uint32 bitmap_height = bitmapRect.bottom - bitmapRect.top;
	uint32 numPixels = bitmap_width * bitmap_height;

	// create file
	ofstream file(filename, ios::binary);
	if(!file) return;

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
	bitmapInfo.bmiHeader.biWidth = bitmap_width;
	bitmapInfo.bmiHeader.biHeight = bitmap_height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 16;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	int bitmapSize = bitmapInfo.bmiHeader.biBitCount/8 * numPixels;

	BYTE* memory = 0;
	HDC memDC =  CreateCompatibleDC(dc);
	HBITMAP bitmap = CreateDIBSection(dc, &bitmapInfo, DIB_RGB_COLORS, (void**)&memory, 0, 0);
	SelectObject(memDC, bitmap);
	BitBlt(memDC, 0, 0, bitmap_width, bitmap_height, dc, 0, 0, SRCCOPY);

	// save dibsection data
	file.write((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
	file.write((char*)&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	file.write((const char*)memory, bitmapSize);
	file.close();

	DeleteDC(memDC);
	DeleteObject(bitmap);
}

//TODO(marcio): FIX font rendering. It is totaly broken!
//TODO: Make the font asset AND the bitmap a single file?
static void saveFontAsset(const char* fileName, uint16 firstChar, uint16 lastChar, uint16 defaultCharacter,
		uint32 bitmapWidth, uint32 bitmapHeight, void* gliphData, uint32 gliphDataSize)
{
	ldk::FontAsset fontAsset;
	fontAsset.rasterWidth = bitmapWidth;
	fontAsset.rasterHeight = bitmapHeight;
	fontAsset.firstCodePoint = firstChar;
	fontAsset.lastCodePoint = lastChar;
	fontAsset.defaultCodePoint = defaultCharacter;
	fontAsset.gliphData = (ldk::FontGliphRect*)sizeof(ldk::FontAsset);

	// create file
	ofstream file(fileName, ios::binary);
	if(!file) return;

	file.write((char*) &fontAsset, sizeof(ldk::FontAsset)); // save font header
	file.write((char*)gliphData, gliphDataSize); 							// save gliph data
	file.close();
}

void enableFontSmoothing()
{
SystemParametersInfo(SPI_SETFONTSMOOTHING,
                     TRUE,
                     0,
                     SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE,
                     0,
                     (PVOID)FE_FONTSMOOTHINGCLEARTYPE,
                     SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}

int _tmain(int argc, _TCHAR** argv)
{
	FontImportInput input;
	char fontName[MAX_FONT_NAME_SIZE];

	if (argc != 4)
	{
		printf("usage:\n makefont TTF-file font-size maxwidth\n");
		return 0;
	}

	input.ttfFontFile = argv[1];
	input.fontSize = atoi(argv[2]);
	input.maxLineWidth = nextPow2(atoi(argv[3])-1);

	// Load font name from the TTF file
	getFontName(input.ttfFontFile, fontName, MAX_FONT_NAME_SIZE);

	uint32 fontWidth = 1024;
	uint32 fontHeight = 1024;
	uint32 dcBitmapSize = 1024 * 1024 * 4;

	// Create device context
	void* bmpMem;
	HDC dc = makeFontDC(&bmpMem, fontWidth, fontHeight);
	memset(bmpMem, 0, dcBitmapSize);

	if (dc == 0)
	{
		LogInfo("Could not create a compatible Device context");
		return 1; 
	}

	// Load the TTF font file
	HFONT hFont = (HFONT)installSystemFontFromTTF(dc, input.ttfFontFile, fontName, input.fontSize);
	if (!hFont)
	{
		LogInfo("Could not load font file %s", input.ttfFontFile);
		return 1;
	}

	// Get some font meterics
	TEXTMETRIC fontMetrics;
	if (!GetTextMetrics(dc, &fontMetrics))
	{
		LogInfo("Could not get metrics for font file %s", input.ttfFontFile);
		return 1; 
	}

	// filling the fontInput structure
	const uint32 spacing = 2;

	input.fontStringLen =  fontMetrics.tmLastChar - fontMetrics.tmFirstChar;
	int8* fontBuffer = new int8[input.fontStringLen];
	input.fontString = (char*)fontBuffer;

	// Create the font string
	for (uint32 i = 0; i < input.fontStringLen; i++ )
	{
		uint8 codePoint = MAX(1, fontMetrics.tmFirstChar + i);

	//TODO: Make sure it happens only for ocidental fonts
	if (codePoint < 32)
		codePoint = fontMetrics.tmDefaultChar;
	else if (codePoint == 127)
		codePoint = fontMetrics.tmDefaultChar;

		fontBuffer[i] = codePoint;
	}

	enableFontSmoothing();

	RECT bitmapRect = calcFontBitmapSize(dc, input.fontString, input.maxLineWidth, spacing);
	HBITMAP hDcBitmap = CreateCompatibleBitmap(dc, bitmapRect.right, bitmapRect.bottom);
	SelectObject(dc, hDcBitmap);
	//SetBkMode(dc, TRANSPARENT);
	SetBkColor(dc, RGB(0,0,0));
	SetTextColor(dc, RGB(255,255,255));

	uint32 gliphX = 0;
	uint32 gliphY = 0;
	char gliph;
	char* fontStringPtr = input.fontString;

	SetBkMode(dc, TRANSPARENT);
	uint32 nextLine = input.fontStringLen/2;

	ldk::FontGliphRect* fontGliphData = new ldk::FontGliphRect[input.fontStringLen];

	//gliphY = bitmapRect.bottom;
	// Output gliphs to the Bitmap
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

		//fontGliphData[i] = {gliphX, bitmapRect.bottom - gliphY - gliphSize.cy, gliphSize.cx, gliphSize.cy};
		//LogInfo("Gliph '%c' (%d) {%d, %d, %d, %d}", gliph, gliph, gliphX, bitmapRect.bottom - gliphY - gliphSize.cy, gliphSize.cx, gliphSize.cy, 0);
		fontGliphData[i] = {gliphX, gliphY, gliphSize.cx, gliphSize.cy};
		LogInfo("Gliph '%c' (%d) {%d, %d, %d, %d}", gliph, gliph, gliphX, gliphY, gliphSize.cx, gliphSize.cy, 0);
		TextOut(dc, gliphX, gliphY, &gliph, 1);
		gliphX += gliphSize.cx + spacing;
	}

	char bmpFileName[128];
	sprintf(bmpFileName, "%s.bmp", fontName);
	saveBitmap(dc, bitmapRect, bmpFileName);

	// Save the asset file
	sprintf(bmpFileName, "%s.font", fontName);
	saveFontAsset(bmpFileName, fontMetrics.tmFirstChar, fontMetrics.tmLastChar, 
			fontMetrics.tmDefaultChar, bitmapRect.right, bitmapRect.bottom, 
			fontGliphData, sizeof(ldk::FontGliphRect) * input.fontStringLen);

	delete fontGliphData;
	delete fontBuffer;
	return 0;
}

