#ifndef __LDARE_TTF__
#define __LDARE_TTF__

#include <string.h>

#define BYTESWAP16(value) (uint16)((((value) & 0x000000FF ) << 8) | (((value) & 0x0000FF00 ) >> 8))

#define BYTESWAP32(value) \
	((((value) & 0x000000FF ) << 24) |\
	 (((value) & 0x0000FF00 ) << 8)   |\
	 (((value) & 0x00FF0000 ) >> 8)   |\
	 (((value) & 0xFF000000 ) >> 24))

struct TTFTableEntry
{
	uint32	tag;
	uint32	checkSum;
	uint32	offset;
	uint32	length;
};

struct TTFFileHeader
{
	uint32	sfntVersion;
	uint16	numTables;
	uint16	searchRange;
	uint16	entrySelector;
	uint16	rangeShift;
};

struct TTFNameTableHeader
{
	uint16	format;
	uint16	count;
	uint16	stringOffset;
};

// if TTRNameTableHeader.format == 0, it points to this structure
struct TTFNameRecord 
{
	uint16	platformID;
	uint16	encodingID;
	uint16	languageID;
	uint16	nameID;
	uint16	length;
	uint16	offset;
};

#define TTF_NAME_FONT_FAMILY_ID 1
#define TTF_NAME_UNIQUE_FONT_ID 3

#define TTF_NAMES_UNLOCALIZED 0
#define TTF_NAMES_LOCALIZED 1

#define MAX_TTF_FONT_NAM_LEN 32
static int8 _fontName[MAX_TTF_FONT_NAM_LEN];

const char* getFontName(TTFNameTableHeader *nameHeader)	
{
	if ( nameHeader->format != TTF_NAMES_UNLOCALIZED)
	{
		LogError("Localized TTF files not supported yet."); 
		return nullptr;
	}

	uint32 numStrings = BYTESWAP16(nameHeader->count);
	TTFNameRecord* nameRecord = (TTFNameRecord*) ((uint8*)nameHeader + sizeof(TTFNameTableHeader));
	uint8* stringStorageStart = ((uint8*)nameHeader) +  sizeof(TTFNameTableHeader) + sizeof(TTFNameRecord) * numStrings;
	for ( uint32 i = 0; i < numStrings; i++)
	{
		if ( BYTESWAP16(nameRecord->nameID) == TTF_NAME_FONT_FAMILY_ID)
		{
			strncpy((char*)_fontName, 
					(const char*) (stringStorageStart + BYTESWAP16(nameRecord->offset)), 
					BYTESWAP16(nameRecord->length));

			break;
		}
		nameRecord++;
	}
	return (const char*) _fontName;
}


#endif // __LDARE_TTF__
