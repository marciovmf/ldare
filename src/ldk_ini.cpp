#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LogUnexpectedToken(expected, line, column)	LogError("Expecting letter while parsing identifier but found %c at %d,%d", (expected), (line), (column));
#define LDK_INI_MAX_IDENTIFIER 128
#define LDK_INI_DEFAULT_BUFFER_SIZE 512
namespace ldk
{
	struct _IniBufferStream
	{
		char8* buffer;
		uint32 line;
		uint32 column;
		uint32 lastColumn;
		char8* pos;
		char8* eofAddr;

		inline bool eof()
		{
			return pos >= eofAddr;
		}

		_IniBufferStream(void* buffer, size_t size):
			buffer((char8*)buffer), line(1), column(1), pos((char8*)buffer), eofAddr((char8*)buffer+size) {}

		char8 peek()
		{
			if (eof())
				return EOF;

			return *pos;
		}

		char8 getc()
		{
			if ( pos >= eofAddr)
				return EOF;

			char8 c = *pos++;
			if (c == '\n')
			{
				++line;
				lastColumn = column; // save it in case of ungetc()
				column = 1;
			}
			else
			{
				lastColumn = column++;
			}
			return c;
		}

		void ungetc()
		{
			if ( pos <= buffer)
				return;

			char8 c = *--pos;
			if ( c == '\n')
			{
				--line;
			}
			column = lastColumn;
		}

	}; 

	enum StatementType
	{
		SECTION_DECLARATION,
		ASSIGNMENT
	};

	struct Identifier
	{
		char8* start;
		uint32 length;
	};

	struct SectionDeclarationStatement
	{
		Identifier sectionName;
	};

	struct Literal
	{
		VariantType type;
		union 
		{
			Identifier stringValue;
			Vec4 vec4Value;
			Vec3 vec3Value;
			float floatValue;
			uint32 intValue;
			uint8 boolValue;
		};
	};

	struct AssignmentStatement
	{
		Identifier identifier;
		Literal value;
	};

	struct Statement
	{
		char8* text;
		uint32 line;
		uint32 column;
		uint32 length;
		StatementType type;

		union
		{
			SectionDeclarationStatement sectionDeclaration;
			AssignmentStatement assignment;
		};

	};

	int32 stringToHash(char8* str)
	{
		uint32 stringLen = strlen((const char*)str);
		int32 hash = 0;
		for (uint32 i = 0; i < stringLen; i++)
		{
			hash += ((char8)* str) * i;
		}

		return hash;
	}
	
	inline bool isLetter(char8 c)
	{
		return (c >= 64 && c <= 90) || (c >= 97 && c <= 122);
	}

	inline bool isDigit(char c)
	{
		return c >= 48 && c <= 57;
	}

	static void skipWhiteSpace(_IniBufferStream& stream)
	{
		char8 c = stream.peek();
		while ( c == ' ' || c == '\t' || c == '\r')
		{
			stream.getc();
			c = stream.peek();
		}
	}

	static bool parseIdentifier(_IniBufferStream& stream, Identifier* identifier)
	{
		char8* identifierText = stream.pos;
		uint32 identifierLength = 0;
		char8 c = stream.getc();

		size_t usedData;

		if (isLetter(c))
		{
			do
			{
				++identifierLength;
				c = stream.getc();
			} while(isLetter(c) || isDigit(c) || c == '_' || c == '-');

			identifier->start = identifierText;
			identifier->length = identifierLength;

			stream.ungetc();
			//TODO: Make sure identifier names are 127bytes or less
			
			if (identifierLength < 128)
			{
				//strncpy((char*)&(identifier->name[0]),(const char*) identifierText, identifierLength);
				return true;
			}

			LogError("Identifier too big at %d,%d while parsing identifier", stream.line, stream.column);
			return false;
		}

		LogError("Unexpected token '%c' at %d,%d while parsing identifier", c, stream.line, stream.column);
		return false;
	}

	// Returns -1 if unary '-', or +1 if unary '+'
	static bool parseUnarySignal(_IniBufferStream& stream, int32* signal)
	{
		char8 c = stream.peek();
		if ( c == '-')
		{
			stream.getc();
			*signal = -1;
			return true;
		}
		else if ( c == '+')
		{
			stream.getc();
			*signal = 1;
			return true;
		}

		*signal = 1;
		return false;
	}

	static bool parseNumericLiteral(_IniBufferStream& stream, Literal* variant)
	{
		int32 signal;
		parseUnarySignal(stream, &signal);
		skipWhiteSpace(stream);

		char8* literalStart = stream.pos;
		uint32 literalLength = 0;
		int8 dotCount = 0;
		char8 c;

		do 
		{
			c = stream.getc();
			if (c == '.') ++dotCount;
			++literalLength;
		} while(isDigit(c) || (c == '.' && dotCount < 2));

		stream.ungetc();
		--literalLength;

		const int maxBuffLen = 32;
		char buff[maxBuffLen]; //I know, I should check the limit of an int/float... someday

		if ( literalLength >= maxBuffLen)
			literalLength = maxBuffLen - 1;

		strncpy(buff, (const char*) literalStart, literalLength);

		if (dotCount > 0)
		{
			variant->type = VariantType::FLOAT;
			variant->floatValue = atof(buff) * signal;
		}
		else
		{
			variant->type = VariantType::INT;
			variant->intValue = atoi(buff) * signal;
		}
		return true;
	}

	static bool parseRValue(_IniBufferStream& stream, Variant* variant)
	{
		skipWhiteSpace(stream);
		int32 signal;
		parseUnarySignal(stream, &signal);
		return false;
	}

	static bool identifierCompare(Identifier& identifier, const char* str)
	{
		int32 length = strlen(str);
		if (length != identifier.length)
			return false;

		for(uint32 i=0; i < identifier.length; i++)
		{
			if (identifier.start[i] != str[i])
				return false;
		}

		return true;
	}

	static bool toBooValue(Identifier& identifier, bool* boolValue)
	{
		uint32 len = identifier.length;

		if (identifierCompare(identifier, "true"))
		{
			*boolValue = true;
			return true;
		}

		if ( identifierCompare(identifier, "false"))
		{
			*boolValue = false;
			return true;
		}

		return false;
	}

	// parse identifier + = + rvalue
	bool parseAssignment(_IniBufferStream& stream, Statement* statement)
	{
		char8 c;
		statement->type = StatementType::ASSIGNMENT;
		Identifier tempIdentifier = {};

		if (parseIdentifier(stream, &statement->assignment.identifier))
		{
			skipWhiteSpace(stream);
			c = stream.getc();
			if ( c == '=')
			{
				skipWhiteSpace(stream);
				c = stream.peek();

				// Bool literal
				if (isLetter(c))
				{
					if (parseIdentifier(stream, &tempIdentifier))
					{
						bool boolValue;
						if (toBooValue(tempIdentifier, &boolValue))
						{
							statement->assignment.value.type = VariantType::BOOL;
							statement->assignment.value.boolValue = (uint8)boolValue;
							return true;
						}

						LogError("Unexpected identifier '%.*s' at %d,%d while parsing assignment", 
								tempIdentifier.length, tempIdentifier.start, stream.line, stream.column);
						return false;
					}
				}

				// string literal
				else if (c == '"')
				{
					uint32 stringLen =0;
					char8* stringStart = stream.pos+1;

					stream.getc();
					do
					{
						++stringLen;
						c = stream.getc();

					}while(c != '"' && c != EOF);
					--stringLen;

					if ( c == '"')
					{
						statement->assignment.value.type = VariantType::STRING;
						statement->assignment.value.stringValue.start = stringStart;
						statement->assignment.value.stringValue.length = stringLen;
						return true;
					}

					LogError("Unexpected EOF at %d,%d while parsing string", stream.line, stream.column);
					return false; // maybe file terminated before string gets closed
				}

				// Nuneric literal
				else if (parseNumericLiteral(stream, &statement->assignment.value))
				{
					return true;
				}
			}
		}
		LogError("Unexpected token '%c' at %d,%d while parsing assignment", c, stream.line, stream.column);
		return false;
	}

	// parse [ + identifier + ]
	static bool parseSectionDeclaration(_IniBufferStream& stream, Statement* statement)
	{
		char8 c = stream.getc();

		if (c == '[')
		{
			skipWhiteSpace(stream);
			if (parseIdentifier(stream, &statement->sectionDeclaration.sectionName))
			{
				skipWhiteSpace(stream);
				c = stream.getc();
				if ( c == ']')
				{
					statement->type = StatementType::SECTION_DECLARATION;
					return true;
				}
			}
		}

		LogError("Error parsing identifier at %d,%d", stream.line, stream.column);
		return false;
	}

	static bool parseStatement(_IniBufferStream& stream, Statement* statement)
	{
		skipWhiteSpace(stream);
		*statement = {};
		statement->line = stream.line;
		statement->column = stream.column;
		bool success = false;

		char8 c = stream.peek();
		if (c == '[')
			success = parseSectionDeclaration(stream, statement);
		else if (isLetter(c))
			success = parseAssignment(stream, statement);
		else
			LogUnexpectedToken(c, stream.line, stream.column);

		skipWhiteSpace(stream);

		// skip comment
		if (success)
		{
			c = stream.getc();
			if (c == '\n')
			{
				return true;
			}
		}
		return false;
	}

	static void skipEmptyLines(_IniBufferStream& stream)
	{
		char8 c;
		do
		{
			skipWhiteSpace(stream);
			c = stream.getc();
		} while (c == '\n');

		if (c != EOF)
			stream.ungetc();
	}

	static uint32 variantDataSize(Literal& literal)
	{
		switch (literal.type)
		{
			case ldk::VariantType::BOOL:
				return sizeof(bool);
				break;
			case ldk::VariantType::INT:
				return sizeof(uint32);
				break;
			case ldk::VariantType::FLOAT:
				return sizeof(float);
				break;
			case ldk::VariantType::STRING:
				return literal.stringValue.length + 1;
				break;
		}

		LogWarning("Unknown literal type");
		return 0;
	}

	// Returns the offset of the section from the start of the heap
	static uint32 pushVariantSection(Heap& heap, Identifier& identifier)
	{
		uint32 necessarySize = sizeof(VariantSection);

		if(heap.free < necessarySize && !ldk_memory_resizeHeap(&heap, necessarySize))
		{
				return -1;
		}

		VariantSection* section = (VariantSection*)((char*)heap.memory + heap.size - heap.free);
		heap.free -= sizeof(VariantSection);

		strncpy((char*)section->name, (char*)identifier.start, identifier.length);
		section->name[identifier.length] = 0;
		section->hash = stringToHash(section->name);
		section->totalSize = sizeof(VariantSection);
		section->variantCount = 0;
		
		// Increment the number of section on the section root
		VariantSectionRoot* sectionRoot = (VariantSectionRoot*) heap.memory;
		++sectionRoot->sectionCount;

		int32 offset = (char*)section - (char*)heap.memory;
		return offset;
	}

	static bool pushVariant(Heap& heap, int32 sectionOffset, Identifier& identifier, Literal& literal)
	{
		uint32 necessarySize = sizeof(Variant) + variantDataSize(literal);


		if(heap.free < necessarySize && !ldk_memory_resizeHeap(&heap, necessarySize))
		{
			return false;
		}
		
		Variant* variant = (Variant*)((char*)heap.memory + heap.size - heap.free);
		heap.free -= necessarySize;

		// update section info
		VariantSection* section = (VariantSection*)(((char*)heap.memory) + sectionOffset);
		section->totalSize += necessarySize;
		++section->variantCount;

		// save variant data into the stream
		strncpy((char*)variant->key,(char*) identifier.start, identifier.length);
		variant->key[identifier.length] = 0;
		variant->hash = stringToHash(variant->key);
		variant->type = literal.type;
		variant->size = necessarySize;

		char* dataStart = ((char*)variant) + sizeof(Variant);

		switch (literal.type)
		{
			case ldk::VariantType::BOOL:
				*((bool*)dataStart) = literal.boolValue;
				break;
			case ldk::VariantType::INT:
				*((int32*)dataStart) = literal.intValue;
				break;
			case ldk::VariantType::FLOAT:
				*((float*)dataStart) = literal.floatValue;
				break;
			case ldk::VariantType::STRING:
				strncpy(dataStart, (char*)literal.stringValue.start, literal.stringValue.length);
				dataStart[literal.stringValue.length] = 0;
				break;
			default:
				return false;
				break;
		}

		return true;
	}

	// returns the offset of the root section
	static uint32 pushRootSection(Heap& heap)
	{
		char* rootSectionName = "/";
		uint32 rootSectionNameLen = strlen(rootSectionName);
		int32 rootSectionOffset = sizeof(ldk::VariantSectionRoot);

		// initialize the number of section on the section root
		VariantSectionRoot* sectionRoot = (VariantSectionRoot*) heap.memory;
		sectionRoot->sectionCount = 0;

		// Account for the root section
		heap.free -= rootSectionOffset;
		Identifier rootSectionIdentifier = {(char8*)rootSectionName, rootSectionNameLen};

		return pushVariantSection(heap, rootSectionIdentifier);
	}

	VariantSectionRoot* ldk_config_parseBuffer(void* buffer, size_t size)
	{
		_IniBufferStream stream(buffer, size);
		Heap heap;
		ldk_memory_allocHeap(&heap, LDK_INI_DEFAULT_BUFFER_SIZE);

		int32 currentSectionOffset = pushRootSection(heap);

		Statement statement;
		bool noError = true;

		while (!stream.eof() && noError)
		{
			skipEmptyLines(stream);
			
			if (stream.peek() == EOF)
				continue;

			if (parseStatement(stream, &statement))
			{
				if ( statement.type == StatementType::SECTION_DECLARATION)
				{
					currentSectionOffset = pushVariantSection(heap, statement.sectionDeclaration.sectionName);
				}
				else if ( statement.type == StatementType::ASSIGNMENT)
				{
					bool isStringAssignment = false;

					pushVariant(heap, currentSectionOffset, 
							statement.assignment.identifier, statement.assignment.value);
				}
			}
			else
			{
				noError = false;
			}
		}

		if (noError)
			return (VariantSectionRoot*) heap.memory;
		else
			return nullptr;
	}

	VariantSectionRoot* ldk_config_parseFile(const char8* fileName)
	{
		size_t fileSize;
		void* buffer = platform::loadFileToBuffer(fileName, &fileSize);

		if (!buffer)
		{
			return nullptr;
		}

		VariantSectionRoot* root = ldk_config_parseBuffer(buffer, fileSize);
		platform::memoryFree(buffer);
		return root;
	}

	VariantSection* ldk_config_getSection(VariantSectionRoot* rootSection, const char* name)
	{
		int32 hash = stringToHash((char8*)name);
		//First section is ALWAYS after the section root
		VariantSection* section = (VariantSection*) ((char*)rootSection + sizeof(VariantSectionRoot));

		for (int i = 0; i < rootSection->sectionCount; i++) 
		{
			if (section->hash == hash && strncmp((char*)section->name, (char*) name, LDK_MAX_IDENTIFIER_SIZE) == 0)
			{
				return section;
			}

			section = (VariantSection*)((char*)section + section->totalSize);
		}
		return nullptr;
	}

	Variant* ldk_config_getVariant(const VariantSection* section, const char* key)
	{
		int32 hash = stringToHash((char8*)key);
		//Variant* v = (ldk::Variant*) (((char*)section) + sizeof(ldk::VariantSection));
		Variant* v = (ldk::Variant*)(section + 1);

		for (int i = 0; i < section->variantCount; i++)
		{
			if ((v->hash == hash) && (strncmp((const char*)key, (const char*)v->key, LDK_MAX_IDENTIFIER_SIZE) == 0))
					return v;

			v = (Variant*)((char*)v + v->size);
		}

		return nullptr;
	}

	bool ldk_config_getInt(VariantSection* section, const char* key, int32* intValue)
	{
		Variant* v = ldk_config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::INT)
		{
			*intValue = *(int*)(v+1);
			return true;
		}

		return false;
	}

	bool ldk_config_getBool(VariantSection* section, const char* key, bool* boolValue)
	{
		Variant* v = ldk_config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::BOOL)
		{
			*boolValue = *(bool*)(v+1);
			return true;
		}

		return false;
	}

	bool ldk_config_getFloat(VariantSection* section, const char* key, float* floatValue)
	{
		Variant* v = ldk_config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::FLOAT)
		{
			*floatValue = *(float*)(v+1);
			return true;
		}

		return false;
	}

	const bool ldk_config_getString(VariantSection* section, const char* key, char** stringValue)
	{
		Variant* v = ldk_config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::STRING)
		{
			*stringValue = ((char*)++v);
			return true;
		}

		return false;
	}

}
