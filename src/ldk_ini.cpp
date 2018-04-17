
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
		uint32 identifierLength = 0;
		char8 c = stream.getc();

		if (isLetter(c))
		{
			do
			{
				++identifierLength;
				c = stream.getc();
			} while(isLetter(c) || isDigit(c) || c == '_' || c == '-');

			stream.ungetc();
			//TODO: Make sure identifier names are 127bytes or less
			strncpy((char*)&(identifier->name[0]),(const char*) identifierText, identifierLength);
			return true;
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

	static bool parseNumericLiteral(_IniBufferStream& stream, Variant* variant)
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

	static bool toBooValue(char8* str, bool* boolValue)
	{
		uint32 len = strlen((char*)str);

		if ( strncmp((char*)str, "true", len) == 0)
		{
			*boolValue = true;
			return true;
		}

		if ( strncmp((char*)str, "false", len) == 0)
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
						if (toBooValue(&(tempIdentifier.name[0]), &boolValue))
						{
							statement->assignment.value.type = VariantType::BOOL;
							statement->assignment.value.boolValue = (uint8)boolValue;
							return true;
						}
						
						LogError("Unexpected identifier '%s' at %d,%d while parsing assignment", 
							tempIdentifier.name, stream.line, stream.column);
						return false;
					}
				}

				// string literal
//				else if (c == '"')
//				{
//					stream.getc();
//					do
//					{
//					}while();
//					stream.getc();
//				}

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
		bool noError = true;

		while (noError)
		{
			skipEmptyLines(stream);

			if (stream.peek() != EOF && parseStatement(stream, &statement))
			{
				if ( statement.type == StatementType::SECTION_DECLARATION)
				{
					LogInfo("%d, %d SECTION DECLARATION  = '%s'", statement.line, statement.column,
							statement.sectionDeclaration.sectionName);
				}
				else if ( statement.type == StatementType::ASSIGNMENT)
				{
					if (statement.assignment.value.type == VariantType::FLOAT)
					{
						LogInfo("%d, %d FLOAT ASSIGNMENT  '%s' = '%f'", statement.line, statement.column,
								statement.assignment.identifier.name, 
								statement.assignment.value.floatValue);
					}
					else if (statement.assignment.value.type == VariantType::INT)
					{
						LogInfo("%d, %d INT ASSIGNMENT  '%s' = '%d'", statement.line, statement.column,
								statement.assignment.identifier.name, 
								statement.assignment.value.intValue);
					}
					else if (statement.assignment.value.type == VariantType::BOOL)
					{
						LogInfo("%d, %d BOOL ASSIGNMENT  '%s' = '%s'", statement.line, statement.column,
								statement.assignment.identifier.name, 
								statement.assignment.value.boolValue ? "true" : "false");
					}
				}
			}
			else
			{
				noError = false;
			}
		}

		return true;
	}
}
