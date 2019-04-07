
namespace ldk
{
  struct TextStreamReader
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

    TextStreamReader(void* buffer, size_t size):
      buffer((char*)buffer),
      line(1),
      column(1),
      pos((char*)buffer),
      eofAddr((char*)buffer+size) {}

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

  //
  // Utility functions
  //

	static void skipWhiteSpace(TextStreamReader& stream)
	{
		char c = stream.peek();
		while ( c == ' ' || c == '\t' || c == '\r')
		{
			stream.getc();
			c = stream.peek();
		}
	}

  static void skipComment(TextStreamReader& stream, const char delim)
	{
		char c = stream.peek();

		if (c != delim)
			return;

		while ( c != '\n')
		{
			stream.getc();
			c = stream.peek();
		}
	}

	static void skipEmptyLines(TextStreamReader& stream)
	{
		char c;
		do
		{
			skipWhiteSpace(stream);
			skipComment(stream, '#');
			c = stream.getc();
		} while (c == '\n');

		if (c != EOF)
			stream.ungetc();
	}
}

