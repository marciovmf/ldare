#ifndef _LDK_BITMAP_H_
#define _LDK_BITMAP_H_

#ifdef _MSC_VER
#pragma pack(push,1)
#endif // _MSC_VER

  #define BITMAP_FILE_HEADER_SIGNATURE 0x4D42
  struct LDK_BITMAP_FILE_HEADER
  {
    uint16	FileType;					/* File type, always 4D42h ("BM") */
    uint32	FileSize;					//* Size of the file in bytes */
    uint16	Reserved1;				//* Always 0 */
    uint16	Reserved2;				//* Always 0 */
    uint32	BitmapOffset;			//* Starting position of image data in bytes */
    uint32	Size;							//* Size of this header in bytes */
    int32	Width;          		//* Image width in pixels */
    int32	Height;         		//* Image height in pixels */
    uint16  Planes;       		//* Number of color planes */
    uint16  BitsPerPixel; 		//* Number of bits per pixel */
    uint32	Compression;  		//* Compression methods used */
    uint32	SizeOfBitmap; 		//* Size of bitmap in bytes */
    int32	HorzResolution; 		//* Horizontal resolution in pixels per meter */
    int32	VertResolution; 		//* Vertical resolution in pixels per meter */
    uint32	ColorsUsed;   		//* Number of colors in the image */
    uint32	ColorsImportant;	//* Minimum number of important colors */
  };
#ifdef _MSC_VER
#pragma pack(pop)
#endif // _MSC_VER

#endif //_LDK_BITMAP_H_
