
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LogUnexpectedToken(expected, line, column)	LogError("Expecting letter while parsing identifier but found %c at %d,%d", (expected), (line), (column));
#define LDK_INI_MAX_IDENTIFIER 128

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

		_IniBufferStream(void* buffer, size_t size):
			buffer((char8*)buffer), line(1), column(1), pos((char8*)buffer), eofAddr((char8*)buffer+size) {}

		char8 peek()
		{
			if ( pos >= eofAddr)
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
		char8 name[LDK_INI_MAX_IDENTIFIER];
	};

	struct SectionDeclarationStatement
	{
		Identifier sectionName;
	};

	struct AssignmentStatement
	{
		Identifier identifier;
		Variant value;
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
		char8 c = stream.getc();

		if (!isLetter(c))
		{
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}

		uint32 identifierLength = 0;
		do
		{
			c = stream.getc();
			++identifierLength;
		}while (isLetter(c) || isDigit(c) || c == '_' || c == '-');

		stream.ungetc();

		//copy identifier name to buffer
		//TODO: Make sure identifier names are 127bytes or less
		strncpy((char*)&(identifier->name[0]),(const char*) identifierText, identifierLength);
		return true;
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
		return false;
	}

	inline bool parseIntLiteral(_IniBufferStream& stream, Variant* variant)
	{
		char8 c = stream.getc();
		if (!isDigit(c))
		{
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}
		
		char8* literalStart = stream.pos;
		uint32 literalLength = 0;

		while(isDigit(c))
		{
			++literalLength;
			c = stream.getc();
		}

		stream.ungetc();

		variant->type = VariantType::INT;
		char buff[16];
		strncpy(buff, (const char*)literalStart, literalLength);
		variant->intValue = atoi(buff);

		return true;
	}

	inline bool parseFractionLiteral(_IniBufferStream& stream, Variant* variant)
	{
		char8 c = stream.getc();

		if (c != '.')
		{
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}
		
		char8* literalStart = stream.pos;
		uint32 literalLength = 0;

		while(isDigit(c))
		{
			++literalLength;
			c = stream.getc();
		}

		stream.ungetc();
		variant->type = VariantType::FLOAT;
		char buff[16];

		strncpy(buff, (const char*)literalStart, literalLength);
		variant->intValue = atof(buff);

		return true;
	}

	static bool parseNumericLiteral(_IniBufferStream& stream, Variant* variant)
	{
		int32 signal;
		parseUnarySignal(stream, &signal);
		skipWhiteSpace(stream);

		char8 c = stream.peek();
		char8* p = stream.pos;
		uint32 literalLength = 0;

		char buff[64];
		c = stream.peek();
	
		// float starting with a dot
		if ( c == '.')
		{
			if (!parseFractionLiteral(stream, variant))
			{
				LogError("Error parsing float literal at %d,%d", stream.line, stream.column);
				return false;
			}

			skipWhiteSpace(stream);
			c = stream.getc();
			if (c != '\n')
			{
				LogUnexpectedToken(c, stream.line, stream.column);
				return false;
			}

			variant->type = VariantType::FLOAT;
		}
		else
		{
			if (!parseIntLiteral(stream, variant))
			{
				LogError("Error parsing numeric literal at %d,%d", stream.line, stream.column);
				return false;
			}

			c = stream.peek();
			if (c == '.')
			{
				int32 wholePart = variant->intValue;
				if (!parseFractionLiteral(stream, variant))
				{
					LogError("Error parsing float literal at %d,%d", stream.line, stream.column);
					return false;
				}
				
				variant->type = VariantType::FLOAT;
				variant->floatValue += wholePart;
			}
			else
			{
				variant->type = VariantType::INT;
			}
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

	// parse identifier + = + rvalue
	bool parseAssignment(_IniBufferStream& stream, Statement* statement)
	{

		// parse identifier
		// getc() == '='
		// parseRValue()
		// getc() == '\n'
		return false;
	}

	// parse [ + identifier + ]
	static bool parseSectionDeclaration(_IniBufferStream& stream, Statement* statement)
	{
		char8 c = stream.getc();
		if (c != '[')
		{
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}

		if (!parseIdentifier(stream, &statement->sectionDeclaration.sectionName))
		{
			LogError("Error parsing identifier at %d,%d", stream.line, stream.column);
			return false;
		}

		c = stream.getc();
		if (c != ']')
		{
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}
	
		skipWhiteSpace(stream);

		c = stream.getc();
		if (c != '\n')
		{
			LogUnexpectedToken(c, stream.line, stream.column);
			return false;
		}

		statement->type = StatementType::SECTION_DECLARATION;
		return true;
	}

	static bool parseStatement(_IniBufferStream& stream, Statement* statement)
	{
		skipWhiteSpace(stream);

		statement->line = stream.line;
		statement->column = stream.column;

		char8 c = stream.peek();
		if (c == '[')
			return parseSectionDeclaration(stream, statement);
		else if (isLetter(c))
			return parseAssignment(stream, statement);
		else
				LogUnexpectedToken(c, stream.line, stream.column);
		
		return false;
	}

	bool ldk_ini_parseFile(const char8* fileName)
	{
		size_t fileSize;
		void* buffer = platform::loadFileToBuffer(fileName, &fileSize);

		if (!buffer)
		{
			return false;
		}

		_IniBufferStream stream(buffer, fileSize);

		Statement statement;
		while (parseStatement(stream, &statement) )
		{
			if ( statement.type == StatementType::SECTION_DECLARATION)
			{
				LogInfo("%d, %d SECTION DECLARATION  = '%s'", statement.line, statement.column,
						statement.sectionDeclaration.sectionName);
			}
		}

		return true;
	}
}
