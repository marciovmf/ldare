
#include <vector>
#include <string.h>
#include "../ldk_textStreamReader.cpp"

namespace ldk
{
  static constexpr uint32 PARESER_MAX_LITERAL_LENGTH = 32;
  static constexpr char PARSER_CHAR_POUND = '#';
  static constexpr char PARSER_CHAR_DOT = '.';
  static constexpr char PARSER_CHAR_SPACE = ' ';
  static constexpr char PARSER_CHAR_MINUS = '-';
  static constexpr char PARSER_CHAR_UNDERLINE = '_';
  static constexpr char PARSER_CHAR_PLUS = '+';
  static constexpr char PARSER_CHAR_SLASH = '/';
  static constexpr char PARSER_CHAR_CARRIAGE_RETURN = '\r';
  static constexpr char PARSER_CHAR_LINE_FEED = '\n';
  static constexpr char PARSER_CHAR_EOF = -1;

  static constexpr char* PARSER_CMD_VERTEX = "v";
  static constexpr char* PARSER_CMD_NORMAL = "vn";
  static constexpr char* PARSER_CMD_UV = "vt";
  static constexpr char* PARSER_CMD_FACE = "f";

  struct ObjStatement
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

  struct Token
  {
    enum
    {
      REAL,
      INTEGER,
      IDENTIFIER,
      SLASH,
      CMD_VERTEX,
      CMD_NORMAL,
      CMD_UV,
      CMD_FACE,
      EOL,
      EOFILE,
    } type;

    char* start;
    uint32 len;
    union
    {
      float floatValue;
      int32 intValue;
    };
  };

  inline bool parseUnarySignal(TextStreamReader& stream, int32* signal)
  {
    char c = stream.peek();
    if ( c == PARSER_CHAR_MINUS)
    {
      stream.getc();
      *signal = -1;
      return true;
    }
    else if ( c == PARSER_CHAR_PLUS)
    {
      stream.getc();
      *signal = 1;
      return true;
    }

    *signal = 1;
    return false;
  }

  static bool parseNumericToken(TextStreamReader& stream, Token& token)
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
      if (c == PARSER_CHAR_DOT) ++dotCount;
      ++literalLength;
    } while(isDigit(c) || (c == '.' && dotCount < 2));

    stream.ungetc();
    --literalLength;

    const int maxBuffLen = PARESER_MAX_LITERAL_LENGTH;
    char buff[maxBuffLen]; 

    if ( literalLength >= maxBuffLen)
      literalLength = maxBuffLen - 1;

    strncpy(buff, (const char*) literalStart, literalLength);

    if (dotCount > 0)
    {
      token.type = Token::REAL;
      token.floatValue = atof(buff) * signal;
    }
    else
    {
      token.type = Token::INTEGER;
      token.intValue = atoi(buff) * signal;
    }
    return true;
  }

  static bool parseIdentifierToken(TextStreamReader& stream, Token& token)
  {
    char* literalStart = stream.pos;
    uint32 literalLength = 0;
    char c;
    do 
    {
      c = stream.getc();
      ++literalLength;
    } while(isLetter(c) || c == PARSER_CHAR_UNDERLINE || c == PARSER_CHAR_MINUS);

    stream.ungetc();
    --literalLength;

    if(strncmp(PARSER_CMD_VERTEX, literalStart, literalLength) == 0)
    {
      token.type = Token::CMD_VERTEX;
    }
    else if(strncmp(PARSER_CMD_NORMAL, literalStart, literalLength) == 0)
    {
      token.type = Token::CMD_NORMAL;
    }
    else if(strncmp(PARSER_CMD_UV, literalStart, literalLength) == 0)
    {
      token.type = Token::CMD_UV;
    }
    else if(strncmp(PARSER_CMD_FACE, literalStart, literalLength) == 0)
    {
      token.type = Token::CMD_FACE;
    }
    else
    {
      token.type = Token::IDENTIFIER;
    }

    token.start = literalStart;
    token.len = literalLength;
    return true;
  }

  static bool getToken(TextStreamReader& stream, Token& token)
  {
    skipWhiteSpace(stream);
    char c = stream.peek();
    if (c == PARSER_CHAR_CARRIAGE_RETURN || c == PARSER_CHAR_LINE_FEED)
    {
      stream.getc();
      token.type = Token::EOL;
      token.len = 1;
      token.start = stream.pos;

      // handle windows line endings
      if (c == PARSER_CHAR_CARRIAGE_RETURN || stream.peek() == PARSER_CHAR_LINE_FEED)
      {
        stream.getc();
        token.len++;
      }
      return true;
    }
    else if (c == PARSER_CHAR_EOF)
    {
      stream.getc();
      token.type = Token::EOFILE;
      token.len = 1;
      token.start = stream.pos;
      return true;
    }
    else if (c == PARSER_CHAR_SLASH)
    {
      stream.getc();
      token.type = Token::EOFILE;
      token.len = 1;
      token.start = stream.pos;
      return true;
    }
    else if (isLetter(c))
    {
      return parseIdentifierToken(stream, token);
    }
    else if (isDigit(c) || c == PARSER_CHAR_MINUS || c == PARSER_CHAR_PLUS || c == PARSER_CHAR_DOT)
    {
      return parseNumericToken(stream, token);
    }

    printf("Error: Unexpected character '%c' at %d,%d", c, stream.line, stream.column);
    return false;
  }

  static bool parseObjFile(const char* fileContent, size_t size)
  {
    Token token;
    bool noError = false;
    TextStreamReader stream((void*)fileContent, size);
    do
    {
      skipEmptyLines(stream);
      skipComment(stream, PARSER_CHAR_POUND);
      noError = getToken(stream, token);
      if(noError)
      {

        switch(token.type)
        {
          case Token::CMD_UV:
          case Token::CMD_NORMAL:
          case Token::CMD_VERTEX:
            Token xValue;
            Token yValue;
            Token zValue;
            if (getToken(stream, xValue) && xValue.type == Token::REAL
                && getToken(stream, yValue) && xValue.type == Token::REAL
                && getToken(stream, zValue) && xValue.type == Token::REAL)
            {
              printf("%.*s %f %f %f\n", token.len, token.start,
                  xValue.floatValue, yValue.floatValue, zValue.floatValue);
            }
            else
              noError = false;
            break;
          case Token::CMD_FACE:
            break;
          default:
            // eat characters untin EOL
            char c;
            do {
              c = stream.getc();
            }
            while(c != PARSER_CHAR_LINE_FEED && c != EOF);
            
            if(c!=EOF) stream.ungetc();
            break;
        }
        printf("Token type %d '%.*s'\n", token.type, token.len, token.start);
      }
    }while(noError && token.type != Token::EOFILE);
    return true;
  }
}
