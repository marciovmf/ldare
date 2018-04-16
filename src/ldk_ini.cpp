
#include <stdio.h>

namespace ldk
{
	struct _IniBufferStream
	{
		char8* buffer;
		uint32 line;
		uint32 column;
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
				column = 1;
			}
			else
			{
				++column;
			}
			return c;
		}

	}; 

	void skipWhiteSpace(_IniBufferStream& stream)
	{
		char8 c = stream.peek();
		while ( c == ' ' || c == '\t' || c == '\r')
		{
			stream.getc();
			c = stream.peek();
		}
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

		uint32 charCount=0;
		char8 c;
		while ((c = stream.getc()) != EOF)
		{
			++charCount;
		}

		LogInfo("lines = %d", stream.line);

		return true;
	}
}
