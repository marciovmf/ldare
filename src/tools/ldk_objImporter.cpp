
#include <string.h>
#include "../ldk_textStreamReader.cpp"

namespace ldk
{
  struct ObjFileStatement
  {
    enum 
    {
      VERTEX,
      NORMAL,
      UV,
      FACE,
      UNSUPORTED
    } type;

    Vec3 position;
    Vec3 normal;
    Vec3 uv;
  };

  struct Literal
  {
    enum
    {
      REAL,
      INTEGER,
      STRING
    } type;
    
    uint32 start;
    uint32 len;
    union
    {
      float floatValue;
      int32 intValue;
    };

  };

  static bool requireCharacter(TextStreamReader& stream, const char required)
  {
    char c = stream.getc();
    return c == required;
  }

	inline bool parseUnarySignal(TextStreamReader& stream, int32* signal)
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

	static bool parseNumericLiteral(TextStreamReader& stream, Literal* literal)
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
		char buff[maxBuffLen]; 

		if ( literalLength >= maxBuffLen)
			literalLength = maxBuffLen - 1;

		strncpy(buff, (const char*) literalStart, literalLength);

		if (dotCount > 0)
		{
			literal->type = Literal::REAL;
			literal->floatValue = atof(buff) * signal;
		}
		else
		{
			literal->type = Literal::INTEGER;
			literal->intValue = atoi(buff) * signal;
		}
		return true;
	}

  static bool parseCommandArgument3f(TextStreamReader& stream, ObjFileStatement& statement)
  {
    skipWhiteSpace(stream);
    char c = stream.peek();

    if (!isDigit(c) && c != '+' && c != '-' && c != '.')
    {
      printf("Error unexpected character '%c' when parsing numeric literal at %d, %d\n",
          c, stream.line, stream.column);
      return false;
    }

    Literal literal1;
    Literal literal2;
    Literal literal3;
    constexpr char SPACE = ' ';

    if (parseNumericLiteral(stream, &literal1) 
        && literal1.type == Literal::REAL 
        && requireCharacter(stream, SPACE)
        // second value
        && parseNumericLiteral(stream, &literal2) 
        && literal2.type == Literal::REAL 
        && requireCharacter(stream, SPACE)
        // third value
        && parseNumericLiteral(stream, &literal3) 
        && literal3.type == Literal::REAL)
      {
        switch(statement.type)
        {
          case ObjFileStatement::VERTEX:
            statement.position = Vec3{literal1.floatValue, literal2.floatValue, literal3.floatValue};
            break;
          case ObjFileStatement::UV:
            statement.uv = Vec3{literal1.floatValue, literal2.floatValue, literal3.floatValue};
            break;
          case ObjFileStatement::NORMAL:
            statement.normal = Vec3{literal1.floatValue, literal2.floatValue, literal3.floatValue};
            break;
          default:
            printf("ERROR: Unknown command\n");
            return false;
            break;
        }
        return true;
      }

    return false;

  }

  static bool parseFaceCommandArgument(TextStreamReader& stream, ObjFileStatement& statement)
  {
    skipWhiteSpace(stream);

    Literal pos;
    Literal uv;
    Literal normal;

    constexpr char SLASH = '/';
    constexpr char SPACE = ' ';

    //TODO(marcio): continue from here...
    return true;
  }

  static bool parseStatement(TextStreamReader& stream, ObjFileStatement& statement)
  {
    constexpr int32 MAX_CMD_LEN = 16; 
    char cmd[MAX_CMD_LEN]={};
    int32 cmdLen = 0;

    while (stream.peek() != ' ')
    {
        cmd[cmdLen++] = stream.getc();
      
        // we exceeded the maximum cmd len. This is an invalid file
        if(cmdLen == MAX_CMD_LEN - 1)
        {
          return false;
        }
    }

    constexpr const char* VERTEX_CMD = "v";
    constexpr const char* NORMAL_CMD = "vn";
    constexpr const char* UV_CMD = "vt";
    constexpr const char* FACE_CMD = "f";
    constexpr const char* MTLIB_CMD = "mtllib";
    constexpr const char* OBJECT_CMD = "o";
    constexpr const char* USEMTL_CMD = "usemtl";
    constexpr const char* SMOOTHGRP_CMD = "s";

    if (strncmp(VERTEX_CMD, cmd, cmdLen) == 0)
    {
      statement.type = ObjFileStatement::VERTEX;
      return parseCommandArgument3f(stream, statement) ;
    }
    else if (strncmp(NORMAL_CMD, cmd, cmdLen) == 0)
    {
      statement.type = ObjFileStatement::NORMAL;
      return parseCommandArgument3f(stream, statement) ;
    }
    else if (strncmp(UV_CMD, cmd, cmdLen) == 0)
    {
      statement.type = ObjFileStatement::UV;
      return parseCommandArgument3f(stream, statement) ;
    }
    //else if (strncmp(FACE_CMD, cmd, cmdLen) == 0)
    //{
    //  statement.type = ObjFileStatement::FACE;
    //  return parseFaceCommandArgument(stream, statement) ;
    //}
    else if (strncmp(MTLIB_CMD, cmd, cmdLen) == 0
        ||strncmp(FACE_CMD, cmd, cmdLen) == 0
        ||strncmp(USEMTL_CMD, cmd, cmdLen) == 0
        ||strncmp(SMOOTHGRP_CMD, cmd, cmdLen) == 0
        ||strncmp(OBJECT_CMD, cmd, cmdLen) == 0)
    {
      statement.type = ObjFileStatement::UNSUPORTED;
      printf("Ignoring unsuported command '%.*s' at %d, %d\n", cmdLen, cmd, stream.line, stream.column);
      //read until EOL
      char c = stream.getc();
      while(c != '\n' && c != EOF)
      {
        c = stream.getc();
      }

      return true;
    }
    else
    {
      // unknown command. This is an invalid file.
      printf("Error: Unknown command '%.*s' at %d, %d", cmdLen, cmd, stream.line, stream.column);
      return false;
    }
  }

  static bool parseObjFile(const char* fileContent, size_t size)
  {
    TextStreamReader stream((void*)fileContent, size);
		bool noError = true;

		while (!stream.eof() && noError)
		{
			skipEmptyLines(stream);
      ObjFileStatement statement;
      noError = parseStatement(stream, statement);

      if(noError && statement.type != ObjFileStatement::UNSUPORTED)
      {
        Vec3* values = nullptr;
        switch(statement.type)
        {
        case ObjFileStatement::VERTEX:
          values = &statement.position;
          break;
        case ObjFileStatement::UV:
          values = &statement.uv;
          break;
        case ObjFileStatement::NORMAL:
          values = &statement.normal;
          break;
        }
      
        if(values != nullptr)
          printf("%d %f %f %f\n", statement.type, values->x, values->y, values->z);
      }

			if (stream.peek() == EOF)
				continue;
    }

    return true;
  }
}

