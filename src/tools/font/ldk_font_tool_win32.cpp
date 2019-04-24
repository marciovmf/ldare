
#include <windows.h>

struct Context
{
  HDC dc;
  HDC tempDc;
  HBITMAP hBitmap;
};

static HGDIOBJ installSystemFontFromTTF(HDC dc, const char* fontFile, const char* fontName, uint32 fontSize)
{
  if (!AddFontResourceEx(fontFile, FR_PRIVATE, 0))
  {
    return 0;
  }

  LOGFONT logFont =
  {
    -MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSX), 72),
    //40,
    //(LONG)-fontSize,
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
    PROOF_QUALITY//DEFAULT_PITCH
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

ldk::Bitmap* dcToBitmap16(HDC dc, RECT bitmapRect, Context& context)
{
  // bitmap dimensions
  const uint32 bitmap_width = bitmapRect.right -  bitmapRect.left;
  const uint32 bitmap_height = bitmapRect.bottom - bitmapRect.top;
  const uint32 numPixels = bitmap_width * bitmap_height;
  const uint32 bitsPerPixel = 32;

  // Final BITMAPINFO to be written to the file
  BITMAPINFO bitmapInfo = {};
  bitmapInfo.bmiHeader.biSize =  sizeof(bitmapInfo.bmiHeader);
  bitmapInfo.bmiHeader.biWidth = bitmap_width;
  bitmapInfo.bmiHeader.biHeight = bitmap_height;
  bitmapInfo.bmiHeader.biPlanes = 1;
  bitmapInfo.bmiHeader.biBitCount = bitsPerPixel;
  bitmapInfo.bmiHeader.biCompression = BI_RGB;

  BYTE* memory = 0;
  HDC tempDc =  CreateCompatibleDC(dc);
  HBITMAP bitmap = CreateDIBSection(dc, &bitmapInfo, DIB_RGB_COLORS, (void**)&memory, 0, 0);
  SelectObject(tempDc, bitmap);
  BitBlt(tempDc, 0, 0, bitmap_width, bitmap_height, dc, 0, 0, SRCCOPY);

  ldk::Bitmap* ldkBmp = new ldk::Bitmap();
  ldkBmp->bitsPerPixel = bitsPerPixel;
  ldkBmp->width = bitmap_width;
  ldkBmp->height = bitmap_height;
  ldkBmp->pixels = (const uchar*)memory;

  context.tempDc = tempDc;
  context.hBitmap = bitmap;
  return ldkBmp;
}

//TODO(marcio): FIX font rendering. It is totaly broken!
//TODO: Make the font asset AND the bitmap a single file?
static void saveFontAsset(const char* fileName, ldk::FontMetrics& metrics,
    uint32 bitmapWidth, uint32 bitmapHeight, void* gliphData, uint32 gliphDataSize)
{
  ldk::FontAsset fontAsset;
  fontAsset.rasterWidth = bitmapWidth;
  fontAsset.rasterHeight = bitmapHeight;
  fontAsset.metrics = metrics;
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

Context initContext()
{
  const uint32 fontWidth = 1024;
  const uint32 fontHeight = 1024;
  const uint32 dcBitmapSize = 1024 * 1024 * 4;

  Context context;

  // Create device context
  void* bmpMem;

  HDC dc = CreateCompatibleDC(GetDC(0));
  BITMAPINFO bmp = {};
  bmp.bmiHeader.biSize = sizeof(bmp.bmiHeader);
  bmp.bmiHeader.biWidth = fontWidth;
  bmp.bmiHeader.biHeight = fontHeight;
  bmp.bmiHeader.biPlanes = 1;
  bmp.bmiHeader.biBitCount = 32;
  bmp.bmiHeader.biCompression = BI_RGB;

  HBITMAP hBitmap = CreateDIBSection(dc, &bmp, DIB_RGB_COLORS, &bmpMem, 0, 0);
  SelectObject(dc, hBitmap);

  if (dc == 0)
  {
    LogInfo("Could not create a compatible Device context");
  }

  memset(bmpMem, 0, dcBitmapSize);
  
  context.dc = dc;
  return context;
}

void destroyContext(Context& context)
{
  DeleteDC(context.dc);
  DeleteDC(context.tempDc);
  DeleteObject(context.hBitmap);
}

ldk::FontMetrics getFontMetrics(const char* ttfFileName, Context& context)
{
  ldk::FontMetrics result;

  // Get some font meterics
  TEXTMETRIC fontMetrics;
  if (!GetTextMetrics(context.dc, &fontMetrics))
  {
    LogError("Could not get metrics for font file %s", ttfFileName);
  }

  result.firstCodePoint = fontMetrics.tmFirstChar;
  result.lastCodePoint = fontMetrics.tmLastChar;
  result.defaultCodePoint = fontMetrics.tmDefaultChar;
  return result;
}

ldk::Bitmap* importFontFromTTF(FontImportSettings& settings, ldk::FontMetrics& metrics, Context& context)
{
  const uint32 spacing = 2;
  HDC dc = context.dc;

  // Load font name from the TTF file
  size_t ttfFileSize;
  void* ttfData = ldk::platform::loadFileToBuffer(settings.ttfFontFile, &ttfFileSize);
  char fontName[MAX_FONT_NAME_SIZE];
  getTTFFontName((uint8*)ttfData, fontName, MAX_FONT_NAME_SIZE);
  ldk::platform::memoryFree(ttfData);

  // Load the TTF font file
  HFONT hFont = (HFONT)installSystemFontFromTTF(dc, settings.ttfFontFile, fontName, settings.fontSize);
  if (!hFont)
  {
    LogInfo("Could not load font file %s", settings.ttfFontFile);
    return nullptr;
  }

  //enableFontSmoothing();

  RECT bitmapRect = calcFontBitmapSize(dc, settings.fontString, settings.maxLineWidth, spacing);
  HBITMAP hDcBitmap = CreateCompatibleBitmap(dc, bitmapRect.right, bitmapRect.bottom);
  SelectObject(dc, hDcBitmap);
  SetBkColor(dc, RGB(0,0,0));
  SetTextColor(dc, RGB(255,255,255));

  uint32 gliphX = 0;
  uint32 gliphY = 0;
  char gliph;
  char* fontStringPtr = settings.fontString;

  SetBkMode(dc, TRANSPARENT);
  uint32 nextLine = settings.fontStringLen/2;

  ldk::FontGliphRect* fontGliphData = new ldk::FontGliphRect[settings.fontStringLen];

  // Output gliphs to the Bitmap
  for (int i=0; i < settings.fontStringLen; i++)
  {
    // TODO: get proper gliph height
    SIZE gliphSize;
    gliph = *fontStringPtr;
    ++fontStringPtr;

    GetTextExtentPoint32(dc, &gliph, 1, &gliphSize);

    // check if we need to break the line
    if (gliphX + gliphSize.cx >= settings.maxLineWidth)
    {
      gliphX = 0;
      gliphY += gliphSize.cy + spacing;
    }
    
    fontGliphData[i] = {gliphX, gliphY, (uint32) gliphSize.cx, (uint32) gliphSize.cy};
    //LogInfo("Gliph '%c' (%d) {%d, %d, %d, %d}", gliph, gliph, gliphX, gliphY, gliphSize.cx, gliphSize.cy, 0);
    TextOut(dc, gliphX, gliphY, &gliph, 1);
    gliphX += gliphSize.cx + spacing;
  }

  char bmpFileName[128];
  sprintf(bmpFileName, "%s.bmp", fontName);
  LogInfo("Saving %s", bmpFileName);

  // Save the asset file
  saveFontAsset(bmpFileName, metrics, bitmapRect.right, bitmapRect.bottom, 
      fontGliphData, sizeof(ldk::FontGliphRect) * settings.fontStringLen);

  return dcToBitmap16(dc, bitmapRect, context);
}

