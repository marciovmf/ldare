#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LogUnexpectedToken(expected, line, column)	LogError("Unexpected token '%c' at %d,%d", (expected), (line), (column));
#define LDK_CFG_MAX_IDENTIFIER_SIZE 63
#define LDK_CFG_MAX_IDENTIFIER 128
#define LDK_CFG_DEFAULT_BUFFER_SIZE 512
namespace ldk
{
	enum VariantType
	{
		INT = 0,
		BOOL,
		FLOAT,
		STRING,
	};

	struct Variant
	{
		char key[LDK_CFG_MAX_IDENTIFIER_SIZE + 1];
		uint32 size;
		VariantType type;
		int32 hash;
		int32 arrayCount;
	};

	struct VariantSection
	{
		int32 hash;
		uint32 variantCount;
		uint32 totalSize; //total size of variant section, including this header
		char name[LDK_CFG_MAX_IDENTIFIER_SIZE + 1];
	};

	struct VariantSectionRoot
	{
		uint32 sectionCount;	
	};

	struct _IniBufferStream
	{
		char* buffer;
		uint32 line;
		uint32 column;
		uint32 lastColumn;
		char* pos;
		char* eofAddr;

		inline bool eof()
		{
			return pos >= eofAddr;
		}

		_IniBufferStream(void* buffer, size_t size):
			buffer((char*)buffer), line(1), column(1), pos((char*)buffer), eofAddr((char*)buffer+size) {}

		char peek()
		{
			if (eof())
				return EOF;

			return *pos;
		}

		char getc()
		{
			if ( pos >= eofAddr)
				return EOF;

			char c = *pos++;
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

			char c = *--pos;
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
		char* start;
		uint32 length;
	};

	struct SectionDeclarationStatement
	{
		Identifier sectionName;
		int32 sectionOffset; // where in the final buffer, this section begins.
	};

	struct Literal
	{
		VariantType type;
		bool isArray;
		union 
		{
			Identifier stringValue;
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
		char* text;
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

	uint32 pushVariantSection(Heap& heap, Identifier& identifier);
	int32 pushVariant(Heap& heap, int32 sectionOffset, Identifier& identifier, Literal& literal);
	int32 pushVariantArrayElement(Heap& heap, int32 sectionOffset, int32 variantOffset, Literal& literal);

	int32 stringToHash(char* str)
	{
		uint32 stringLen = strlen((const char*)str);
		int32 hash = 0;
		for (uint32 i = 0; i < stringLen; i++)
		{
			hash += ((char)* str) * i;
		}

		return hash;
	}

	inline bool isLetter(char c)
	{
		return (c >= 64 && c <= 90) || (c >= 97 && c <= 122);
	}

	inline bool isDigit(char c)
	{
		return c >= 48 && c <= 57;
	}

	static void skipWhiteSpace(_IniBufferStream& stream)
	{
		char c = stream.peek();
		while ( c == ' ' || c == '\t' || c == '\r')
		{
			stream.getc();
			c = stream.peek();
		}
	}

	static void skipComment(_IniBufferStream& stream)
	{
		char c = stream.peek();

		if (c != '#')
			return;

		while ( c != '\n')
		{
			stream.getc();
			c = stream.peek();
		}
	}

	static void skipEmptyLines(_IniBufferStream& stream)
	{
		char c;
		do
		{
			skipWhiteSpace(stream);
			skipComment(stream);
			c = stream.getc();
		} while (c == '\n');

		if (c != EOF)
			stream.ungetc();
	}

	static bool parseIdentifier(_IniBufferStream& stream, Identifier* identifier)
	{
		char* identifierText = stream.pos;
		uint32 identifierLength = 0;
		char c = stream.getc();

		size_t usedData;

		if (isLetter(c) || c == '_')
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
		char c = stream.peek();
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

	static bool parseNumericLiteral(_IniBufferStream& stream, Literal* literal)
	{
		int32 signal;
		parseUnarySignal(stream, &signal);
		skipWhiteSpace(stream);

		char* literalStart = stream.pos;
		uint32 literalLength = 0;
		int8 dotCount = 0;
		char c;

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
			literal->type = VariantType::FLOAT;
			literal->floatValue = atof(buff) * signal;
		}
		else
		{
			literal->type = VariantType::INT;
			literal->intValue = atoi(buff) * signal;
		}
		return true;
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

	inline bool parseStringLiteral(_IniBufferStream& stream, Literal& literal)
	{
		uint32 stringLen = 0;
		char* stringStart = stream.pos+1;
		char c = stream.peek();

		if (c != '"')
		{
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}

		stream.getc();
		do
		{
			++stringLen;
			c = stream.getc();

		}while(c != '"' && c != EOF);
		--stringLen;

		if ( c == '"')
		{
			literal.type = VariantType::STRING;
			literal.stringValue.start = stringStart;
			literal.stringValue.length = stringLen;
			//return true;
		}
		else
		{
			LogError("Unexpected EOF at %d,%d while parsing string", stream.line, stream.column);
			return false; // maybe file terminated before string gets closed
		}

		return true;
	}

	//
	// Parse single RValue, except Arrays
	// It does NOT persist the parsed value
	//
	static bool parseSingleRValue(Heap& parsedDataBuffer,
			_IniBufferStream& stream, Identifier& identifier, Literal& literal)
	{
		skipWhiteSpace(stream);
		char c = stream.peek();
		// Bool literal
		if (isLetter(c))
		{
			Identifier tempIdentifier = {};
			if (parseIdentifier(stream, &tempIdentifier))
			{
				bool boolValue;
				if (toBooValue(tempIdentifier, &boolValue))
				{
					literal.type = VariantType::BOOL;
					literal.boolValue = (uint8)boolValue;
				}
				else
				{
					LogError("Unexpected identifier '%.*s' at %d,%d while parsing assignment", 
							tempIdentifier.length, tempIdentifier.start, stream.line, stream.column);
					return false;
				}
			}
		}
		// String literal
		else if (c == '"')
		{
			parseStringLiteral(stream, literal);
		}
		// Nuneric literal
		else if (isDigit(c) || c == '+' || c == '-' || c == '.')
		{
			parseNumericLiteral(stream, &literal);
		}
		else
		{
			// unknown
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}

		return true;
	}

	static bool parseRValue(Heap& parsedDataBuffer,
			_IniBufferStream& stream, Identifier& identifier, Literal& literal, int32 currentSectionOffset)
	{
		skipEmptyLines(stream);
		char c = stream.peek();

		int32 variantOffset;

		// is it an array ?
		if (c == '[')
		{
			c = stream.getc();
			ldk::VariantType arrayType;
			int32 elementCount = 0;

			do 
			{
				skipEmptyLines(stream);

				if (parseSingleRValue(parsedDataBuffer, stream, identifier, literal))
				{
					// fist element sets the array type
					if (elementCount == 0)
					{
						arrayType = literal.type;
						literal.isArray = true;
						variantOffset = pushVariant(parsedDataBuffer, currentSectionOffset, identifier, literal); 
					}
					else 
					{
						if ( arrayType != literal.type)
						{
							LogError("Invalid mixed type array at %d,%d", stream.line, stream.column);
							return false;
						}
						// save the new element into the final array
						pushVariantArrayElement(parsedDataBuffer, currentSectionOffset, variantOffset, literal);
					}

					//update array count on variant
					++elementCount;
					skipEmptyLines(stream);
					c = stream.getc(); // Get the ',' separating the next element
				}
				else
				{
					return false;
				}
			} while (c == ',');

			if (c == EOF)
			{
				LogError("Unexpected EOF while parsing array at %d,%d", stream.line, stream.column);
				return false;
			}

			if (c != ']')
			{
				LogError("Unexpected token '%c' while looking for array termination at %d, %d", 
						c, stream.line, stream.column);

				return false;
			}
			
			return true;
		}
		// is it a single variant ?
		else 
		{
			if (parseSingleRValue(parsedDataBuffer, stream, identifier, literal))
				return pushVariant(parsedDataBuffer, currentSectionOffset, identifier, literal) > 0;
			else
				return false;
		}
	}

	// parse identifier + '=' + rvalue
	bool parseAssignment(Heap& parsedDataBuffer, _IniBufferStream& stream, Statement* statement, int32 currentSectionOffset)
	{
		char c;
		statement->type = StatementType::ASSIGNMENT;
		Identifier tempIdentifier = {};

		if (parseIdentifier(stream, &statement->assignment.identifier))
		{
			skipWhiteSpace(stream);
			c = stream.getc();
			if ( c == '=')
			{ 
				return parseRValue(parsedDataBuffer, 
						stream, 
						statement->assignment.identifier,
						statement->assignment.value,
						currentSectionOffset);
			}
			LogUnexpectedToken(c, stream.line, stream.column);
		}
		return false;
	}

	// parse [ + identifier + ]
	static bool parseSectionDeclaration(Heap& parsedDataBuffer, _IniBufferStream& stream, Statement* statement)
	{
		char c = stream.getc();

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

					// save the section data into the final buffer
					statement->sectionDeclaration.sectionOffset = 
						pushVariantSection(parsedDataBuffer, statement->sectionDeclaration.sectionName);

					return true;
				}
			}
		}

		LogError("Error parsing identifier at %d,%d", stream.line, stream.column);
		return false;
	}

	static bool parseStatement(Heap& parsedDataBuffer, _IniBufferStream& stream, Statement* statement, int32 currentSectionOffset)
	{
		skipWhiteSpace(stream);
		skipComment(stream);

		*statement = {};
		statement->line = stream.line;
		statement->column = stream.column;
		bool success = false;

		char c = stream.peek();
		if (c == '[')
			success = parseSectionDeclaration(parsedDataBuffer, stream, statement);
		else if (isLetter(c))
			success = parseAssignment(parsedDataBuffer, stream, statement, currentSectionOffset);
		else
			LogUnexpectedToken(c, stream.line, stream.column);

		skipWhiteSpace(stream);

		skipComment(stream);

		if (success)
		{
			c = stream.getc();
			if (c == '\n' || c == EOF)
			{
				return true;
			}
		}
		return false;
	}

	static uint32 variantDataSize(Literal& literal)
	{
		// ignore the array bit
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
		int32 availableSize = heap.size - heap.used;

		if(availableSize < necessarySize && !ldk_memory_resizeHeap(&heap, necessarySize))
		{
			return -1;
		}

		VariantSection* section = (VariantSection*)((char*)heap.memory + heap.used);

		heap.used += sizeof(VariantSection);

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

	static int32 pushVariantArrayElement(Heap& heap, int32 sectionOffset, int32 variantOffset, Literal& literal)
	{
	 // adds a new array element
		uint32 necessarySize = variantDataSize(literal);
		int32 availableSize = heap.size - heap.used;

		if(availableSize < necessarySize && !ldk_memory_resizeHeap(&heap, necessarySize))
		{
			return -1;
		}

		heap.used += necessarySize;

		VariantSection* section = (VariantSection*) ((char*) heap.memory + sectionOffset);
		Variant* variant = (Variant*)((char*)heap.memory + variantOffset);

		LDK_ASSERT((variant->arrayCount > 0 ),
				"Can not add element to a non array variant");

		LDK_ASSERT((variant->type == literal.type),
				"Can not add element to array of different type");

		char* elementPosition = ((char*) variant) + variant->size;
		switch (variant->type)
		{
			case ldk::VariantType::BOOL:
				*(bool*)elementPosition = (bool)literal.boolValue;
				break;
			case ldk::VariantType::INT:
				*(int32*)elementPosition = (int32)literal.intValue;
				break;
			case ldk::VariantType::FLOAT:
				*(float*)elementPosition = (float)literal.floatValue;
				break;
			case ldk::VariantType::STRING:
				//*elementPosition = (char*)literal.stringValue;
				//TODO: we must store a list of string offsets before all strings. Probably as a post processing step.
				//For this to work, I must add an extra sizeof(char*) to each string and then relocate them to the end
				LDK_ASSERT(false,"string array is not implemented yet");
				break;
			default:
				LogError("Can not persist unknow array element type");
				return -1;
				break;
		}
		
		// updates the section total size
		section->totalSize += necessarySize;
		// increments the variant array element count
		variant->size += necessarySize;
		variant->arrayCount++;
		return variantOffset;
	}

	// Saves a variant parsed data and returns the offset of the variant
	static int32 pushVariant(Heap& heap, int32 sectionOffset, Identifier& identifier, Literal& literal)
	{
		uint32 necessarySize = sizeof(Variant) + variantDataSize(literal);
		int32 availableSize = heap.size - heap.used;

		if(availableSize < necessarySize && !ldk_memory_resizeHeap(&heap, necessarySize))
		{
			return -1;
		}

		Variant* variant = (Variant*)((char*)heap.memory + heap.used );
		heap.used += necessarySize;
		int32 variantOffset = (int32)((char*) variant - (char*)heap.memory);

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
		variant->arrayCount = literal.isArray ? 1 : -1; // -1 means it is not an array

		char* dataStart = (char*)(++variant);

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
				return -1;
				break;
		}

		// return offset of variant
		return variantOffset;
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
		heap.used += rootSectionOffset;
		Identifier rootSectionIdentifier = {(char*)rootSectionName, rootSectionNameLen};

		return pushVariantSection(heap, rootSectionIdentifier);
	}

	VariantSectionRoot* config_parseBuffer(void* buffer, size_t size)
	{
		_IniBufferStream stream(buffer, size);
		Heap heap;
		ldk_memory_allocHeap(&heap, LDK_CFG_DEFAULT_BUFFER_SIZE);

		int32 currentSectionOffset = pushRootSection(heap);

		Statement statement;
		bool noError = true;

		while (!stream.eof() && noError)
		{
			skipEmptyLines(stream);

			if (stream.peek() == EOF)
				continue;

			if (parseStatement(heap, stream, &statement, currentSectionOffset))
			{
				if ( statement.type == StatementType::SECTION_DECLARATION)
				{
					currentSectionOffset = statement.sectionDeclaration.sectionOffset;
				}
				//else if ( statement.type == StatementType::ASSIGNMENT)
				//{
				//pushVariant(heap, currentSectionOffset, 
				//statement.assignment.identifier, statement.assignment.value);
				//}
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

	VariantSectionRoot* config_parseFile(const char* fileName)
	{
		size_t fileSize;
		void* buffer = platform::loadFileToBuffer(fileName, &fileSize);
		LogInfo("Loading config file '%s'", fileName);
		if (!buffer)
		{
			return nullptr;
		}

		VariantSectionRoot* root = config_parseBuffer(buffer, fileSize);
		platform::memoryFree(buffer);
		return root;
	}

	VariantSection* config_getSection(VariantSectionRoot* rootSection, const char* name)
	{
		int32 hash = stringToHash((char*)name);
		//First section is ALWAYS after the section root
		VariantSection* section = (VariantSection*) ((char*)rootSection + sizeof(VariantSectionRoot));

		for (int i = 0; i < rootSection->sectionCount; i++) 
		{
			if (section->hash == hash && strncmp((char*)section->name, (char*) name, LDK_CFG_MAX_IDENTIFIER_SIZE) == 0)
			{
				return section;
			}

			section = (VariantSection*)((char*)section + section->totalSize);
		}
		return nullptr;
	}

	Variant* config_getVariant(const VariantSection* section, const char* key)
	{
		int32 hash = stringToHash((char*)key);
		//Variant* v = (ldk::Variant*) (((char*)section) + sizeof(ldk::VariantSection));
		Variant* v = (ldk::Variant*)(section + 1);

		for (int i = 0; i < section->variantCount; i++)
		{
			if ((v->hash == hash) && (strncmp((const char*)key, (const char*)v->key, LDK_CFG_MAX_IDENTIFIER_SIZE) == 0))
				return v;

			v = (Variant*)((char*)v + v->size);
		}

		return nullptr;
	}

	int32 config_getArray(VariantSection* section, const char* key, void** array, VariantType type)
	{
		Variant* v = config_getVariant(section, key);
		int32 arraySize = v->arrayCount;
		if (v != nullptr && arraySize >= 0)
		{
			*array = ++v; // array data 
			return arraySize;
		}
		return -1;
	}
	
	bool config_getInt(VariantSection* section, const char* key, int32* intValue)
	{
		Variant* v = config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::INT)
		{
			*intValue = *(int*)(v+1);
			return true;
		}

		return false;
	}

	bool config_getBool(VariantSection* section, const char* key, bool* boolValue)
	{
		Variant* v = config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::BOOL)
		{
			*boolValue = *(bool*)(v+1);
			return true;
		}

		return false;
	}

	bool config_getFloat(VariantSection* section, const char* key, float* floatValue)
	{
		Variant* v = config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::FLOAT)
		{
			*floatValue = *(float*)(v+1);
			return true;
		}

		return false;
	}

	const bool config_getString(VariantSection* section, const char* key, char** stringValue)
	{
		Variant* v = config_getVariant(section, key);
		if (v != nullptr && v->type == VariantType::STRING)
		{
			*stringValue = ((char*)++v);
			return true;
		}

		return false;
	}

	int32 config_getIntArray(VariantSection* section, const char* key, int32** array)
	{
		return config_getArray(section, key,(void**) array, VariantType::INT);
	}

	int32 config_getFloatArray(VariantSection* section, const char* key, float** array)
	{
		return config_getArray(section, key,(void**) array, VariantType::FLOAT);
	}

	int32 config_getBoolArray(VariantSection* section, const char* key, bool** array)
	{
		return config_getArray(section, key,(void**) array, VariantType::BOOL);
	}

	void config_dispose(VariantSectionRoot* root)
	{
		ldk::ldk_memory_freeHeap((Heap*) root);
	}
}
