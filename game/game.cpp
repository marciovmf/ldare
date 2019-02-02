#include <ldk/ldk.h>
using namespace ldk;

//TODO(marcio): Yikes! remove these!
#include <stdlib.h>
#include <ctime>
#include <algorithm> 

static const uint32 MAX_SPRITES = 256;
static const uint32 GAME_GRID_SIZE = 8;
static const uint32 SCREEN_HEIGHT = 768;
static const uint32 GAME_BOARD_OFFSET_X = 0;
static const uint32 GAME_BOARD_OFFSET_Y = SCREEN_HEIGHT;
static const uint32 GAME_MOVE_ANIMATION_SPEED = 400;
static const uint32 GAME_SCALE_ANIMATION_SPEED = 5;
static const uint32 GAME_TIME_LIMIT_SECONDS = 60;
static const uint32 GAME_GRID_PIECE_SIZE = 45;

#define STR(s) #s
// Vertex shader
char* vs = STR(#version 330 core\n
    in vec3 _pos; 
    in vec2 _uuv; 
    out vec2 fragCoord;
    uniform mat4 mmodel;
    uniform mat4 mprojection;
    void main()
    {
    gl_Position = mprojection * mmodel * vec4(_pos, 1.0); \n
    fragCoord = _uuv;
    });

// Fragment shader
char* fs = STR(#version 330 core\n
    in vec2 fragCoord;
    uniform sampler2D _mainTexture;
    out vec4 out_color;

    void main()
    {
    vec4 solidColor = vec4(1.0, 1.0, 1.0, 1.0); 

    vec4 textureColor = texture(_mainTexture, fragCoord);
    out_color = mix(solidColor, textureColor, 0.9);
    });

enum
{
  YELLOW = 0,
  GREEN,
  BLUE,
  PURPLE,
  RED,
  BACKGROUND,
  NUM_SPRITES
};

// prites rects
ldk::Rect spriteRect[] =
{
	//Yellow
	{755,426,38,37},
	//Green
	{863,426,35,35},
	//Blue
	{793,426,35,36},
	//Purple
	{828,426,35,35},
	//Red.
	{898,426,34,36},
	//BackGround
	{0,426,755,600},
};

// In game piece
struct Piece
{
  enum PieceType
  {
    YELLOW = 0,
    GREEN,
    BLUE,
    PURPLE,
    RED,
    PICE_TYPE_COUNT
  };

  PieceType type;
  float x;
  float y;
  float scale;
  bool match;

  Piece(){ }

  Piece(float x, float y)
    : x(x)
      , y(y)
      , scale(1.0f)
      , match(false)
  {
    SetRandomPieceType();
  }

  void SetRandomPieceType()
  {
    type = (Piece::PieceType)(rand() % (int)Piece::PieceType::PICE_TYPE_COUNT);
  }
};

// Global game state variables
static struct GameState
{
  uint32 initialized;
  renderer::Sprite sprite[NUM_SPRITES];
  renderer::Material material;
  renderer::Texture textureId;
  renderer::Context* context;
  renderer::SpriteBatch* spriteBatch;
  Mat4 modelMatrix;
  Mat4 projMatrix;
  // gameplay state
  Piece* pieceA;
  Piece* pieceB;
  int score;
  int bestScore;
  int clickCount;
  float remainingGameTime;
  bool isSwapping;
  bool isAnimating;
  bool isGameOver;
  Piece grid[GAME_GRID_SIZE][GAME_GRID_SIZE];
} *_gameState;

inline renderer::Sprite PieceTypeToTexture(Piece::PieceType type)
{
  return _gameState->sprite[(int32) type];
}

inline Piece* getPieceUnderCursor(int cursorX, int cursorY)
{
  const int32 halfPieceSize = (int32) (GAME_GRID_PIECE_SIZE / 2);
  int column = (cursorX - GAME_BOARD_OFFSET_X - GAME_GRID_PIECE_SIZE) / GAME_GRID_PIECE_SIZE;
  int row = (GAME_BOARD_OFFSET_Y - cursorY - halfPieceSize) / GAME_GRID_PIECE_SIZE;

  if (row >= 0 && row < GAME_GRID_SIZE &&
      column >= 0 && column < GAME_GRID_SIZE)
  {
    return &_gameState->grid[column][row];
  }

  return nullptr;
}

void Swap(Piece& pieceA, Piece& pieceB)
{
  std::swap(_gameState->pieceA->x, _gameState->pieceB->x);
  std::swap(_gameState->pieceA->y, _gameState->pieceB->y);
  std::swap(_gameState->pieceA->type, _gameState->pieceB->type);
}

/*
 * Animate pieces towards their ideal position.
 * Returns true if at least one pice is animating this frame
 */
inline bool UpdatePieceMovementAnimation(float deltaTime, int speed)
{
  bool isAnimating = false;
  const float step = speed * deltaTime;

  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      Piece& piece = _gameState->grid[column][row];

      float idealX = (float)column * GAME_GRID_PIECE_SIZE;
      float idealY = (float)row * GAME_GRID_PIECE_SIZE;

      float deltaX = piece.x - idealX;
      float deltaY = piece.y - idealY;

      const float onePixelThreshold = 1.0f;
      if ((std::abs(deltaX) < onePixelThreshold) && (std::abs(deltaY) < onePixelThreshold))
      {
        piece.x = idealX;
        piece.y = idealY;
        continue;
      }

      int directionX = (int)(deltaX / std::abs(deltaX));
      int directionY = (int)(deltaY / std::abs(deltaY));

      if (deltaX) piece.x -= directionX * step;
      if (deltaY) piece.y -= directionY * step;

      if (deltaX || deltaY)
      {
        isAnimating = true;
      }
    }
  }
  return isAnimating;
}

/*
 * Animate pieces match animation.
 * Returns true if at least one pice is animating this frame
 */
inline bool UpdatePieceMatchAnimation(float deltaTime, int speed)
{
  bool isScaling = false;
  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      Piece& piece = _gameState->grid[column][row];
      if (piece.match)
      {
        if (piece.scale > 0.0f)
        {
          piece.scale = MAX(0.0f, piece.scale - speed * deltaTime);
          isScaling = true;
        }
      }
    }
  }
  return isScaling;
}

/*
 * Marks matched pieces as 'matched'
 */
inline void UpdateMatchedPieces()
{
  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      Piece& piece = _gameState->grid[column][row];

      if (row > 0 && row < GAME_GRID_SIZE - 1)
      {
        Piece& vPieceBefore = _gameState->grid[column][row - 1];
        Piece& vPieceAfter = _gameState->grid[column][row + 1];
        if (vPieceBefore.type == piece.type && vPieceAfter.type == piece.type)
        {
          piece.match = true;
          vPieceBefore.match = true;
          vPieceAfter.match = true;
        }
      }

      if (column > 0 && column < GAME_GRID_SIZE - 1)
      {

        Piece& hPieceBefore = _gameState->grid[column - 1][row];
        Piece& hPieceAfter = _gameState->grid[column + 1][row];
        if (hPieceBefore.type == piece.type && hPieceAfter.type == piece.type)
        {
          piece.match = true;
          hPieceBefore.match = true;
          hPieceAfter.match = true;
        }
      }
    }
  }
}

int GetScore()
{
  int score = 0;
  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      Piece& piece = _gameState->grid[column][row];
      score += (int)piece.match;
    }
  }
  return score;
}

/*
 * Updates the grid recycling matched pieces and dropping new ones
 */
inline void UpdateGrid()
{
  bool mustDropNewPieces = false;

  // copy unmatched pieces down to their new position
  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = GAME_GRID_SIZE - 1; row >= 0; row--)
    {				
      if (_gameState->grid[column][row].match)
      {
        int matchRow = row;
        for (int i = row; i >= 0; i--)
        {
          Piece& fallingPiece = _gameState->grid[column][i];

          if (!fallingPiece.match)
          {
            Piece& fallTarget = _gameState->grid[column][matchRow--];
            fallTarget = fallingPiece;
            fallingPiece.match = true;
          }
        }
      }
    }
  }

  // drop new pieces
  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    int pieceOffset = 0;
    for (int row = GAME_GRID_SIZE - 1; row >= 0; row--)
    {
      Piece& newPiece = _gameState->grid[column][row];
      if (newPiece.match)
      {
        newPiece.match = false;
        newPiece.SetRandomPieceType();
        newPiece.y -= (row + pieceOffset++ + 1) * GAME_GRID_PIECE_SIZE;
        newPiece.scale = 1.0f;
      }
    }
  }
}

/*
 * Updates gamplay input. returns true if player starts sapping pieces this frame
 */
inline bool UpdateSwapInput()
{
  bool trySwapping = false;

  bool buttonDown = ldk::input::getMouseButtonDown(ldk::input::LDK_MOUSE_LEFT);
  bool buttonHold = ldk::input::getMouseButton(ldk::input::LDK_MOUSE_LEFT);
  Vec2 cursor = ldk::input::getMouseCursor();

  Piece* pieceUnderCursor = getPieceUnderCursor((int)cursor.x, (int)cursor.y);

  if (buttonDown)
  {
    if (pieceUnderCursor)
    {
      ++_gameState->clickCount;
      if (_gameState->clickCount == 1)
      {
        _gameState->pieceA = pieceUnderCursor;
      }
      else
      {
        _gameState->pieceB = pieceUnderCursor;
        trySwapping = true;
        _gameState->clickCount = 0;
      }
    }
    else
    {
      _gameState->clickCount = 0;
    }
  }
  else if (buttonHold && (pieceUnderCursor && _gameState->pieceA && pieceUnderCursor != _gameState->pieceA))
  {
    _gameState->clickCount = 0;
    trySwapping = true;
  }

  if (trySwapping)
  {
    _gameState->pieceB = pieceUnderCursor;
    float dx = std::abs(_gameState->pieceA->x - pieceUnderCursor->x);
    float dy = std::abs(_gameState->pieceA->y - pieceUnderCursor->y);

    if ((dx == GAME_GRID_PIECE_SIZE && dy == 0) || (dy == GAME_GRID_PIECE_SIZE && dx == 0))
    {
      Swap(*_gameState->pieceA, *_gameState->pieceB);
      _gameState->clickCount = 0;
      return true;
    }			
  }
  return false;
}

void UpdateGameTimer(float deltaTime)
{
  _gameState->remainingGameTime -= deltaTime;
  if (_gameState->remainingGameTime <= 0)
  {
    _gameState->isGameOver = true;
    if (_gameState->score > _gameState->bestScore) _gameState->bestScore = _gameState->score;
  }
}

inline void DrawGameplay()
{
  Vec2 cursor = ldk::input::getMouseCursor();
  Piece* pieceUnderCursor = getPieceUnderCursor((int)cursor.x, (int)cursor.y);
  //if(pieceUnderCursor) LogInfo("Cursor %d %d", (int)cursor.x, (int)cursor.y);

  ldk::renderer::spriteBatchBegin(_gameState->spriteBatch);

  const int halfPieceSize = GAME_GRID_PIECE_SIZE / 2;

//  for (int column = 0; column < GAME_GRID_SIZE; column++)
//  {
//  	for (int row = 0; row < GAME_GRID_SIZE; row++)
//  	{
//      float angle = 0.0f;
//      Piece& piece = _gameState->grid[column][row];
//  		const auto sprite = PieceTypeToTexture(piece.type);
//
//      //if( row == 0 ) scale = 0.5f;
//      //else 
//        if (&piece == pieceUnderCursor)
//        angle = RADIAN(90.0f);
//
//      ldk::renderer::spriteBatchDraw(_gameState->spriteBatch,
//          &sprite,
//          GAME_BOARD_OFFSET_X + piece.x + halfPieceSize,
//          GAME_BOARD_OFFSET_Y - piece.y - halfPieceSize, 1.0f, 1.0f, angle);
//  	}
//  
//  }

    ldk::renderer::spriteBatchDraw(_gameState->spriteBatch,
        &_gameState->sprite[BACKGROUND], 
        0,
        0,//GAME_BOARD_OFFSET_Y,
        1.0f,
        1.0f);

  //std::string elasedTimeText = std::to_string((int)_gameState->remainingGameTime);
  //mEngine.Write("Time:", 100, 50);
  //mEngine.Write(elasedTimeText.c_str(), 100, 100);
  //mEngine.Write("Score:", 100, 150);

  //std::string scoreText = std::to_string(_gameState->score);
  //mEngine.Write(scoreText.c_str(), 100, 200);
  ldk::renderer::spriteBatchEnd(_gameState->spriteBatch);
}

inline void DrawGameOver()
{
  //mEngine.Render(EXTURE_BACKGROUND, 0, 0);
  //const char* clickToPlayText = "Click to start new game";
  //float textWidth = mEngine.CalculateStringWidth(clickToPlayText);
  //float textAreaCenter = GAME_GRID_PIECE_SIZE * GAME_GRID_SIZE * 0.5f;
  //mEngine.Write(clickToPlayText, GAME_BOARD_OFFSET_X + textAreaCenter - textWidth * 0.5f, GAME_BOARD_OFFSET_Y + textAreaCenter);

  //std::string bestScoreString = "Best score:" + std::to_string(_gameState->bestScore);
  //const char* bestScoreText = bestScoreString.c_str();
  //textWidth = mEngine.CalculateStringWidth(bestScoreText);
  //mEngine.Write(bestScoreText, GAME_BOARD_OFFSET_X + textAreaCenter - textWidth * 0.5f, GAME_BOARD_OFFSET_Y + 100.0f);

}

inline void resetInput()
{
  _gameState->pieceA = nullptr;
  _gameState->pieceB = nullptr;
  _gameState->clickCount = 0;
}

void newGame()
{
  _gameState->remainingGameTime = GAME_TIME_LIMIT_SECONDS;
  _gameState->score = 0;
  _gameState->isGameOver = false;
  _gameState->isSwapping = false;
  _gameState->isAnimating = false;
  _gameState->clickCount = 0;

  unsigned int seed = (unsigned int)std::time(0);
  std::srand(seed);

  const int32 halfPieceSize = GAME_GRID_PIECE_SIZE / 2;
  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      Piece& piece = Piece((float)column * GAME_GRID_SIZE , (float)row * GAME_GRID_SIZE );

      bool hMatchRisk = false;
      bool vMatchRisk = false;
      Piece::PieceType hPreviousType;
      Piece::PieceType vPreviousType;

      // Prevent starting the board with matches
      if (column >= 2 && (_gameState->grid[column - 1][row].type == _gameState->grid[column - 2][row].type))
      {
        hPreviousType = _gameState->grid[column - 1][row].type;
        hMatchRisk = true;
      }

      if (row >= 2 && (_gameState->grid[column][row - 1].type == _gameState->grid[column][row - 2].type))
      {
        vPreviousType = _gameState->grid[column][row - 1].type;
        vMatchRisk = true;
      }

      while ((hMatchRisk && hPreviousType == piece.type)
          || (vMatchRisk && vPreviousType == piece.type))
      {
        piece.SetRandomPieceType();
      }
      _gameState->grid[column][row] = piece;
    }
  }
}


//
// LDK Callbacks
//

LDKGameSettings gameInit()
{
  LDKGameSettings settings = ldk::loadGameSettings();
  settings.preallocMemorySize = sizeof(GameState);
  return settings;
}

void gameStart(void* memory)
{
  _gameState = (GameState*)memory;

  srand((uint64)memory << 32);

  if (_gameState->initialized)
    return;

  _gameState->context = renderer::createContext(255, renderer::Context::COLOR_BUFFER | renderer::Context::DEPTH_BUFFER, 0);

  // Load Texture from bmp
  auto bmp = loadBitmap("Assets/sprites.bmp");
  _gameState->textureId = renderer::createTexture(bmp);
  freeAsset((void*) bmp);

  // Initialize material
  makeMaterial(&_gameState->material, vs, fs);
  renderer::setTexture(&_gameState->material, "_mainTexture", _gameState->textureId); 

  // Calculate matrices and send them to shader uniforms  
  // projection 
  _gameState->projMatrix.orthographic(0, 1024, 0, 768, -10, 10);
  renderer::setMatrix4(&_gameState->material, "mprojection", &_gameState->projMatrix);
  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{1.0, 1.0, 1.0});
  _gameState->modelMatrix.translate(Vec3{0, 0, -2});
  renderer::setMatrix4(&_gameState->material, "mmodel", &_gameState->modelMatrix);

  // Initialize the sprite batch
  _gameState->spriteBatch = renderer::createSpriteBatch( _gameState->context, MAX_SPRITES); // Max 32 sprites, for now...

  // initialize sprites
  for (uint32 i =0; i < NUM_SPRITES; i++)
  {
    renderer::makeSprite(&_gameState->sprite[i], 
        &_gameState->material,
        spriteRect[i].x,
        spriteRect[i].y,
        spriteRect[i].w,
        spriteRect[i].h);
  }
}


void gameUpdate(float deltaTime)
{
  // gameover screen
  if (_gameState->isGameOver)
  {			
    
    if (ldk::input::getMouseButtonUp(ldk::input::LDK_MOUSE_LEFT))
    {
      newGame();
    }
    DrawGameOver();
    return;
  }

  // gameplay timer
  UpdateGameTimer(deltaTime);

  // gameplay imput
  if (!_gameState->isSwapping && !_gameState->isAnimating)
  {
    _gameState->isSwapping = UpdateSwapInput();
  }

  UpdateMatchedPieces();
  int score = GetScore();

  _gameState->isAnimating = UpdatePieceMovementAnimation(deltaTime, GAME_MOVE_ANIMATION_SPEED)
    || UpdatePieceMatchAnimation(deltaTime, GAME_SCALE_ANIMATION_SPEED);

  if (!_gameState->isAnimating)
  {
    if (_gameState->isSwapping)
    {
      if (score == 0)
      {
        Swap(*_gameState->pieceA, *_gameState->pieceB);
      }

      _gameState->isSwapping = false;
      resetInput();
    }
    else
    {
      UpdateGrid();
    }

    _gameState->score += score;			
  }

  DrawGameplay();
}


void gameStop()
{
  ldk::renderer::destroyTexture(_gameState->textureId);
  ldk::renderer::destroyContext(_gameState->context);
  ldk::renderer::destroySpriteBatch(_gameState->spriteBatch);
}
