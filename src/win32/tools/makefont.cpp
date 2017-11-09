#include <ldare/ldare.h>
#include <windows.h>
#include <tchar.h>
#include <fstream>

using namespace std;

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

int _tmain(int argc, _TCHAR** argv)
{

	if (argc != 2)
	{
		LogInfo("usage:\n makefont TTF-file");
		return 0;
	}

	char* fontFile = argv[1];
	TEXTMETRIC fontMetrics;
	HDC dc;
  const char* fontString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-:,[{()}]";
	SIZE fontStringSize;
	int32 fontStringLen = strlen(fontString);
	HBITMAP bitmapFont;
  HFONT hFont;

	if (!AddFontResourceEx(fontFile, FR_PRIVATE, 0))
	{
		LogInfo("Could not load font file %s", fontFile);
		return 1; 
	}

	dc = CreateCompatibleDC(GetDC(0));
	//dc = GetDC(0);
	if (dc == 0)
	{
		LogInfo("Could not create a compatible Device context");
		return 1; 
	}

	if (!GetTextMetrics(dc, &fontMetrics))
	{
		LogInfo("Could not get metrics for font file %s", fontFile);
		return 1; 
	}

	const uint32 spacing = 2;

 LOGFONT myFont;
	RECT rect = {};
	//TODO: select the current font!
	GetTextExtentPoint32(dc, fontString, fontStringLen, &fontStringSize);
	rect.right = fontStringSize.cx + fontStringLen * spacing;
	rect.bottom = fontStringSize.cy;
	HBITMAP hDcBitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom);
	SelectObject(dc, hDcBitmap);
	SetTextColor(dc, RGB(255,255,255));
	SetBkColor(dc,RGB(0,0,0));

	uint32 gliphX = 0;
	char gliph;
	char* gliphPtr = (char*)fontString[1];

	for (int i=0; i < fontStringLen; i++)
	{
		// TODO: get proper gliph height
		SIZE gliphSize;
		gliph = fontString[i];
		//gliphPtr++;
		GetTextExtentPoint32(dc, &gliph, 1, &gliphSize);
		LogInfo("Gliph '%c' (%d) {%d, %d, %d, %d}", gliph, gliph, gliphX, 0, gliphSize.cx, gliphSize.cy, 0);
		TextOut(dc, gliphX, 0, &gliph, 1);
		gliphX += gliphSize.cx + spacing;
	}

	saveBitmap(dc,rect,"font.bmp");
	return 0;
}
