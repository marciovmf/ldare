#ifndef __LDK_TTF__
#define __LDK_TTF__

#include <string.h>

// Byte ordering macros
#define BYTESWAP16(value) (uint16)((((value) & 0x000000FF ) << 8) | (((value) & 0x0000FF00 ) >> 8))

#define BYTESWAP32(value) \
	((((value) & 0x000000FF ) << 24) |\
	 (((value) & 0x0000FF00 ) << 8)   |\
	 (((value) & 0x00FF0000 ) >> 8)   |\
	 (((value) & 0xFF000000 ) >> 24))

// 'name' header 
#define TTF_NAME_TABLE 0x656D616E
#define TTF_CMAP_TABLE 0x70616D63

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

// name table
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

// cmap table
struct TTFCmapHeader
{
	uint16 version;
	uint16 numTables;
};

struct TTFEncodingRecord
{
	uint16 platformId; 				// Expected to be 3 - Windows
	uint16 encodingId; 				// Expected to be 1 - Unicode BMP (UCS-2)
	uint32 offset;
};

// String ids for getTTFString()
#define TTF_NAME_FONT_FAMILY_ID 1
#define TTF_NAME_UNIQUE_FONT_ID 3

uint32 getTTFString(const TTFNameTableHeader *nameHeader, 
		char* outputBuffer, 
		uint32 bufferSize, 
		uint32 stringId)	
{
	// 0 means unlocalized strings, 1 means localized strings
	if ( nameHeader->format != 0)
	{
		LogError("Localized TTF files not supported yet."); 
		return 0;
	}

	uint32 stringSize = 0;
	uint32 numStrings = BYTESWAP16(nameHeader->count);
	TTFNameRecord* nameRecord = (TTFNameRecord*) ((uint8*)nameHeader + sizeof(TTFNameTableHeader));
	uint8* stringStorageStart = 
		((uint8*)nameHeader) + sizeof(TTFNameTableHeader) + sizeof(TTFNameRecord) * numStrings;

	for ( uint32 i = 0; i < numStrings; i++)
	{
		if ( BYTESWAP16(nameRecord->nameID) == stringId)
		{
			stringSize = MIN(BYTESWAP16(nameRecord->length), bufferSize);
			strncpy(outputBuffer, (const char*) (stringStorageStart + BYTESWAP16(nameRecord->offset)), stringSize);
			break;
		}
		nameRecord++;
	}
	return stringSize;
}

void* getTTFTablePtr(const uint8* data, uint32 tableName)
{
	// Look for HEAD table
	TTFFileHeader *ttfHeader = (TTFFileHeader*)data;

	// start of table entries
	TTFTableEntry* tableEntry = (TTFTableEntry*) (data + sizeof(TTFFileHeader));

	// find name table
	uint32 numTables = BYTESWAP16(ttfHeader->numTables);
	//int8 nameTag[4] = { 'n', 'a', 'm', 'e' };
	for (uint32 i = 0; i < numTables; i++)
	{
		TTFTableEntry* entry = tableEntry;
		tableEntry++;
		// check entry table name
		//if (*((uint32*)nameTag) == entry->tag)
		if (tableName == entry->tag)
		{
			return (void*)(data + BYTESWAP32(entry->offset));
		}
	}

	return nullptr;
}

#endif // __LDK_TTF__
