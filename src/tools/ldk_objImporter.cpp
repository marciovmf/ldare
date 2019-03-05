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

  struct ObjIndex
  {
    int32 vIndex;
    int32 vtIndex;
    int32 vnIndex;
  };

  struct Token
  {
    enum TokenType
    {
      CMD_VERTEX = 0, // these explicit values are used for array indexing.
      CMD_NORMAL = 1,
      CMD_UV = 2,
      CMD_FACE = 3,
      SPACE,
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
      token.type = Token::TokenType::REAL;
      token.floatValue = atof(buff) * signal;
    }
    else
    {
      token.type = Token::TokenType::INTEGER;
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
      token.type = Token::TokenType::CMD_VERTEX;
    }
    else if(strncmp(PARSER_CMD_NORMAL, literalStart, literalLength) == 0)
    {
      token.type = Token::TokenType::CMD_NORMAL;
    }
    else if(strncmp(PARSER_CMD_UV, literalStart, literalLength) == 0)
    {
      token.type = Token::TokenType::CMD_UV;
    }
    else if(strncmp(PARSER_CMD_FACE, literalStart, literalLength) == 0)
    {
      token.type = Token::TokenType::CMD_FACE;
    }
    else
    {
      token.type = Token::TokenType::IDENTIFIER;
    }

    token.start = literalStart;
    token.len = literalLength;
    return true;
  }

  static bool getToken(TextStreamReader& stream, Token& token)
  {
    char c = stream.peek();
    if (c == PARSER_CHAR_CARRIAGE_RETURN || c == PARSER_CHAR_LINE_FEED)
    {
      stream.getc();
      token.type = Token::TokenType::EOL;
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
    else if (c == PARSER_CHAR_SPACE)
    {
      stream.getc();
      token.type = Token::TokenType::SPACE;
      token.len = 1;
      token.start = stream.pos;
      return true;
    }
    else if (c == PARSER_CHAR_EOF)
    {
      stream.getc();
      token.type = Token::TokenType::EOFILE;
      token.len = 1;
      token.start = stream.pos;
      return true;
    }
    else if (c == PARSER_CHAR_SLASH)
    {
      stream.getc();
      token.type = Token::TokenType::SLASH;
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

  static bool requireToken(TextStreamReader& stream, Token::TokenType tokenType, Token& token)
  {
    bool result = getToken(stream, token) && token.type == tokenType;
    if(!result) printf("Parsing error: Expected token %d but found %d at %d, %d\n",
        tokenType, token.type, stream.line, stream.column);
    return result;
  }

  enum FaceTripletType
  {
    V,
    V_VT,
    V_VN,
    V_VT_VN,
    UNKNOWN = -1
  };

  static bool parseFaceTripletAndGetType(TextStreamReader& stream, 
      int& v, int& vt, int& vn, FaceTripletType& tripletType)
  {
    //NOTE(marcio): This function does not consume the space after the first triplet!
    Token t1, t2;
    if(requireToken(stream, Token::TokenType::INTEGER, t1))
    {
      v = t1.intValue;
      TextStreamReader streamCheckpoint = stream; // save the stream state at this point

      if(getToken(stream, t1))
      {
        // V
        if(t1.type == Token::TokenType::SPACE)
        {
          stream = streamCheckpoint; // restore the stream state to unget the last token 
          vt = vn = 0;
          tripletType = FaceTripletType::V;
          return true;
        }

        if(t1.type == Token::TokenType::SLASH && getToken(stream, t1) && getToken(stream, t2))
        {
          // V//VN
          if(t1.type == Token::TokenType::SLASH
              && t2.type == Token::TokenType::INTEGER)
          {
            vt = 0;
            vn = t2.intValue;
            tripletType = FaceTripletType::V_VN;
            return true;
          }

          // V/VT
          streamCheckpoint = stream;
          if(t1.type == Token::TokenType::INTEGER
              && t2.type == Token::TokenType::SPACE)
          {
            stream = streamCheckpoint; // restore the stream state to unget the last token 
            vt = 0;
            vn = t2.intValue;
            tripletType = FaceTripletType::V_VN;
            return true;
          }

          // V/VT/VN
          if(t1.type == Token::TokenType::INTEGER
              && t2.type == Token::TokenType::SLASH
              && requireToken(stream, Token::TokenType::INTEGER, t2))
          {
            vt = 0;
            vn = t2.intValue;
            tripletType = FaceTripletType::V_VN;
            return true;
          }

        }
      }
    }
    return false;
  }

  static bool requireFaceTripletOfType(TextStreamReader& stream, 
      int32& v, int32& vt, int32& vn, FaceTripletType tripletType)
  {
    Token vToken;
    Token vtToken;
    Token vnToken;
    Token slashToken;
    
    switch (tripletType)
    {
      case FaceTripletType::V:
        if (requireToken(stream, Token::TokenType::INTEGER, vToken))
        {
          v = vToken.intValue;
          vt = 0;
          vn = 0;
          return true;
        }
        break;
        
      case FaceTripletType::V_VT:
        if (requireToken(stream, Token::TokenType::INTEGER, vToken)
            && requireToken(stream, Token::TokenType::SLASH, slashToken)
            && requireToken(stream, Token::TokenType::INTEGER, vtToken))
        {
          v = vToken.intValue;
          vt = vtToken.intValue;
          vn = 0;
        }
        break;

      case FaceTripletType::V_VN:
        if (requireToken(stream, Token::TokenType::INTEGER, vToken)
            && requireToken(stream, Token::TokenType::SLASH, slashToken)
            && requireToken(stream, Token::TokenType::SLASH, slashToken)
            && requireToken(stream, Token::TokenType::INTEGER, vnToken))
        {
          v = vToken.intValue;
          vt = 0;
          vn = vnToken.intValue;
          return true;
        }
        break;

      case FaceTripletType::V_VT_VN:
        if (requireToken(stream, Token::TokenType::INTEGER, vToken)
            && requireToken(stream, Token::TokenType::SLASH, slashToken)
            && requireToken(stream, Token::TokenType::INTEGER, vtToken)
            && requireToken(stream, Token::TokenType::SLASH, slashToken)
            && requireToken(stream, Token::TokenType::INTEGER, vnToken))
        {
          v = vToken.intValue;
          vt = vtToken.intValue;
          vn = vnToken.intValue;
          return true;
        }
        break;
    }

    return false;
  }

  inline bool requireVec3f(TextStreamReader& stream, Vec3& vec3)
  {
    Token xValue;
    Token yValue;
    Token zValue;
    Token temp;
    if (requireToken(stream, Token::TokenType::REAL, xValue)
        && requireToken(stream, Token::TokenType::SPACE, temp)
        && requireToken(stream, Token::TokenType::REAL, yValue)
        && requireToken(stream, Token::TokenType::SPACE, temp)
        && requireToken(stream, Token::TokenType::REAL, zValue)
        && requireToken(stream, Token::TokenType::EOL, temp))
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
    std::vector<ObjIndex> objIndices;
    uint32 triangleCount = 0;

    FaceTripletType faceFormat = FaceTripletType::UNKNOWN;

    do
    {
      skipEmptyLines(stream);
      skipComment(stream, PARSER_CHAR_POUND);
      noError = getToken(stream, token);
      Token tempToken;
      if(noError)
      {
        Vec3 tempVec;
        switch(token.type)
        {
          case Token::TokenType::CMD_UV:
            noError = requireToken(stream, Token::TokenType::SPACE, tempToken)
              && requireVec3f(stream, tempVec); 
              if (noError) uvs.push_back(tempVec);
              break;
         
          case Token::TokenType::CMD_NORMAL:
            noError = requireToken(stream, Token::TokenType::SPACE, tempToken)
              && requireVec3f(stream, tempVec);
              if(noError) normals.push_back(tempVec);
              break;
         
          case Token::TokenType::CMD_VERTEX:
              noError = requireToken(stream, Token::TokenType::SPACE, tempToken)
                && requireVec3f(stream, tempVec);
              if(noError) vertices.push_back(tempVec);
              break;
         
          case Token::CMD_FACE:
            {
              triangleCount++;
              int v=0, vt=0, vn=0;

              noError = requireToken(stream, Token::TokenType::SPACE, tempToken);
              if(!noError) break;

              // detects the format of the faces
              if(faceFormat == FaceTripletType::UNKNOWN)
              {
                TextStreamReader streamCheckpoint = stream;
                noError = parseFaceTripletAndGetType(stream, v, vt, vn, faceFormat);
                if (!noError) break;
                stream = streamCheckpoint;
              }

              for (int i = 0; i < 3; i++) 
              {
                ObjIndex objIndex;
                noError = requireFaceTripletOfType(stream, objIndex.vIndex, objIndex.vtIndex, objIndex.vnIndex, faceFormat);
                if (noError && getToken(stream, token))
                {
                  //TODO(triangulate face if it has more than 3 faces)
                  if(i < 2 && token.type != Token::TokenType::SPACE
                      || i == 2 && (token.type != Token::TokenType::EOL && token.type != Token::TokenType::EOFILE))
                  {
                    noError = false;
                    break;
                  }
                }

                objIndices.push_back(objIndex);
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


    if (!noError)
    {
      printf("Syntax error parsing obj file at %d, %d\n", stream.line, stream.column);
    }

    printf("Status = %s, Vertex count = %zd, uv count = %zd, Normal count = %zd Triangle count %d\n",
        noError ? "success" : "failed" , vertices.size(), uvs.size(), normals.size(), triangleCount);

    return true;
  }
}
