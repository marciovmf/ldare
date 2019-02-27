
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

  struct Token
  {
    enum
    {
      CMD_VERTEX = 0, // these explicit values are used for array indexing.
      CMD_NORMAL = 1,
      CMD_UV = 2,
      CMD_FACE = 3,
      REAL,
      INTEGER,
      IDENTIFIER,
      SLASH,
      EOL,
      EOFILE,
      UNKNOWN
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

  inline bool requireVertexNormalUvIndex(TextStreamReader& stream, 
      int& vertexIndex, int& uvIndex, int& normalIndex)
  {
    char* empty = "''";
    Token vertex;
    Token normal;
    Token uv;

    vertex.start = normal.start = uv.start = empty;
    vertex.len = normal.len = uv.len = 2;
    vertex.type = normal.type = uv.type = Token::UNKNOWN;


    Token slash;

    if (getToken(stream, vertex) && vertex.type == Token::INTEGER 
        && getToken(stream, slash) && slash.type == Token::SLASH

        && getToken(stream, normal) && normal.type == Token::INTEGER
        && getToken(stream, slash) && slash.type == Token::SLASH

        && getToken(stream, uv) && uv.type == Token::INTEGER)
    {
      vertexIndex = vertex.intValue;
      normalIndex = normal.intValue;
      uvIndex = uv.intValue;
      return true;
    }

    printf("error\n");
    printf("vertex Token %d, %.*s \n", vertex.type, vertex.len, vertex.start);
    printf("normal Token %d, %.*s \n", normal.type, normal.len, normal.start);
    printf("uv Token %d, %.*s \n", uv.type, uv.len, uv.start);

    return false;
  }

  inline bool requireVec3(TextStreamReader& stream, Vec3& vec3)
  {
    Token xValue;
    Token yValue;
    Token zValue;
    if (getToken(stream, xValue) && xValue.type == Token::REAL
        && getToken(stream, yValue) && xValue.type == Token::REAL
        && getToken(stream, zValue) && xValue.type == Token::REAL)
    {

      vec3.x = xValue.floatValue;
      vec3.y = xValue.floatValue;
      vec3.z = xValue.floatValue;
      return true;
    }
    return false;
  }

  static bool parseObjFile(const char* fileContent, size_t size)
  {
    Token token;
    bool noError = false;
    TextStreamReader stream((void*)fileContent, size);

    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec3> uvs;
    std::vector<Vec3> output;
    uint32 triangleCount = 0;
    
    do
    {
      skipEmptyLines(stream);
      skipComment(stream, PARSER_CHAR_POUND);
      noError = getToken(stream, token);
      if(noError)
      {
        Vec3 tempVec;
        switch(token.type)
        {
          case Token::CMD_UV:
            noError = requireVec3(stream, tempVec); 
              if (noError )uvs.push_back(tempVec);
              break;
         
          case Token::CMD_NORMAL:
              noError = requireVec3(stream, tempVec);
              if(noError) normals.push_back(tempVec);
              break;
         
          case Token::CMD_VERTEX:
              requireVec3(stream, tempVec);
                if(noError) vertices.push_back(tempVec);
            break;
         
          case Token::CMD_FACE:
            {
              int vertexIndex=0;
              int normalIndex=0;
              int uvIndex=0;

              triangleCount++;

              for (int i=0; i < 3; i++)
              {
                noError = requireVertexNormalUvIndex(stream, vertexIndex, uvIndex, normalIndex);
                if(!noError)
                {
                  printf("Error parsing face part %d\n",i);
                  triangleCount--;
                  break;
                }

                output.push_back(vertices[vertexIndex]);
                output.push_back(uvs[uvIndex]);
                output.push_back(normals[normalIndex]);
              }
            }
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
      }
    }while(noError && token.type != Token::EOFILE);


    printf("Status = %s, Vertex count = %zd, uv count = %zd, Normal count = %zd Triangle count %d\n",
        noError ? "success" : "failed" , vertices.size(), uvs.size(), normals.size(), triangleCount);

    return true;
  }
}
