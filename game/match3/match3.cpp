//NOTE(marcio): Game logic consides the board grows top bottom. The larger the
//piece Y value, the lower the piece screen position (since sprite batch draws bottom up)
//sprite origin is bottom left
//screen origin is bottom left
//mouse position origin is bottom left
//Game grid row 0 is top row.

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
static const uint32 GAME_SCALE_ANIMATION_SPEED = 5;
static const uint32 GAME_TIME_LIMIT_SECONDS = 180;
static const uint32 GAME_GRID_PIECE_SIZE = 100;
static const float GAME_MOVE_ANIMATION_SPEED = 10.0f;

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
  float column; // column
  float row; // row
  float scale;
  bool match;

  Piece(){ }

  Piece(float column, float row)
    : column(column)
      , row(row)
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
  HMaterial material;
  renderer::Texture textureId;
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

inline renderer::Sprite pieceTypeToTexture(Piece::PieceType type)
{
  return _gameState->sprite[(int32) type];
}

inline void pieceToScreenPosition(float row, float column, Vec2* position)
{
  position->x = column * GAME_GRID_PIECE_SIZE;
  position->y = GAME_GRID_SIZE * (GAME_GRID_PIECE_SIZE) - (row + 1) * GAME_GRID_PIECE_SIZE;
}

inline Piece* getPieceUnderCursor(int cursorX, int cursorY)
{
  const int totalGridSize = (GAME_GRID_PIECE_SIZE * GAME_GRID_SIZE);
  int column = (cursorX * GAME_GRID_SIZE)/ totalGridSize;
  int row = GAME_GRID_SIZE - 1 - ((cursorY * GAME_GRID_SIZE)/ totalGridSize);

  if (row >= 0 && row < GAME_GRID_SIZE &&
      column >= 0 && column < GAME_GRID_SIZE)
  {
    return &_gameState->grid[column][row];
  }

  return nullptr;
}

static void swap(Piece& pieceA, Piece& pieceB)
{
  std::swap(_gameState->pieceA->column, _gameState->pieceB->column);
  std::swap(_gameState->pieceA->row, _gameState->pieceB->row);
  std::swap(_gameState->pieceA->type, _gameState->pieceB->type);
}

int getScore()
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

//******************************************************************************
// Animate pieces towards their ideal position.
// Returns true if at least one pice is animating this frame
//******************************************************************************
inline bool updatePieceMovementAnimation(float deltaTime, float speed)
{
  bool isAnimating = false;
  const float step = speed * deltaTime;

  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      Piece& piece = _gameState->grid[column][row];

      float idealX = (float)column;
      float idealY = (float)row;

      float deltaX = piece.column - idealX;
      float deltaY = piece.row - idealY;


      if(deltaX == 0 && deltaY == 0)
      {
        continue;
      }

      if (deltaX)
      {
        int directionX = (int) (deltaX / std::abs(deltaX));
        piece.column -= directionX * step;

        if (directionX > 0 && piece.column < column
            || directionX < 0 && piece.column > column)
        {
          piece.column = column;
        }
      }
   
      if (deltaY)
      {
        int directionY = (int) (deltaY / std::abs(deltaY));
        float p = piece.row - directionY * step;
      
        if ((piece.row > row && p < row)
            || (piece.row < row && p > row))
        {
          piece.row = row;
        }
        else
        {
          piece.row = p;
        }
      }

      if (deltaX || deltaY)
      {
        isAnimating = true;
      }
    }
  }
  return isAnimating;
}

//******************************************************************************
// Animate pieces match animation.
// Returns true if at least one pice is animating this frame
//******************************************************************************
inline bool updatePieceMatchAnimation(float deltaTime, int speed)
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

//******************************************************************************
// Marks matched pieces as 'matched'
//******************************************************************************
inline void updateMatchedPieces()
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

//******************************************************************************
// Updates the grid recycling matched pieces and dropping new ones
//******************************************************************************
inline void updateGrid()
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
        newPiece.row -= (row + pieceOffset++ + 1) ;//* GAME_GRID_PIECE_SIZE;
        newPiece.scale = 1.0f;
      }
    }
  }
}

//******************************************************************************
// Updates gamplay input. returns true if player starts sapping pieces this frame
//******************************************************************************
inline bool updateSwapInput()
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
    float dx = std::abs(_gameState->pieceA->column - pieceUnderCursor->column);
    float dy = std::abs(_gameState->pieceA->row - pieceUnderCursor->row);

    //if ((dx == GAME_GRID_PIECE_SIZE && dy == 0) || (dy == GAME_GRID_PIECE_SIZE && dx == 0))
    if ((dx == 1.0f && dy == 0) || (dy == 1.0f && dx == 0))
    {
      swap(*_gameState->pieceA, *_gameState->pieceB);
      _gameState->clickCount = 0;
      return true;
    }			
  }
  return false;
}

//******************************************************************************
// Updat game timer
//******************************************************************************
void updateGameTimer(float deltaTime)
{
  _gameState->remainingGameTime -= deltaTime;
  if (_gameState->remainingGameTime <= 0)
  {
    _gameState->isGameOver = true;
    if (_gameState->score > _gameState->bestScore)
      _gameState->bestScore = _gameState->score;
  }
}

//******************************************************************************
// draw 
//******************************************************************************
inline void drawGameplay()
{

  renderer::clearBuffers(renderer::Context::COLOR_BUFFER | renderer::Context::DEPTH_BUFFER);

  Vec2 cursor = ldk::input::getMouseCursor();
  Piece* pieceUnderCursor = getPieceUnderCursor((int)cursor.x, (int)cursor.y);

  ldk::renderer::spriteBatch_begin(_gameState->spriteBatch);

  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      float highlight = 0.0f;
      Piece& piece = _gameState->grid[column][row];
  		const auto sprite = pieceTypeToTexture(piece.type);

      if (&piece == pieceUnderCursor)
        highlight = 3.0f;

      Vec2 piecePos;
      pieceToScreenPosition(piece.row, piece.column, &piecePos);

      ldk::renderer::spriteBatch_draw(_gameState->spriteBatch
          ,&sprite
          ,piecePos.x + (GAME_GRID_PIECE_SIZE / 2) * (1 - piece.scale)
          ,piecePos.y + highlight + (GAME_GRID_PIECE_SIZE / 2) * (1 - piece.scale)
          ,GAME_GRID_PIECE_SIZE * piece.scale
          ,GAME_GRID_PIECE_SIZE * piece.scale
          ,0);
  	}
  
  }

//    ldk::renderer::spriteBatchDraw(_gameState->spriteBatch,
//        &_gameState->sprite[BACKGROUND], 
//        0,
//        0,//GAME_BOARD_OFFSET_Y,
//        1.0f,
//        1.0f);

  //std::string elasedTimeText = std::to_string((int)_gameState->remainingGameTime);
  //mEngine.Write("Time:", 100, 50);
  //mEngine.Write(elasedTimeText.c_str(), 100, 100);
  //mEngine.Write("Score:", 100, 150);

  //std::string scoreText = std::to_string(_gameState->score);
  //mEngine.Write(scoreText.c_str(), 100, 200);
  ldk::renderer::spriteBatch_end(_gameState->spriteBatch);
}

inline void drawGameOver()
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

  for (int column = 0; column < GAME_GRID_SIZE; column++)
  {
    for (int row = 0; row < GAME_GRID_SIZE; row++)
    {
      Piece& piece = Piece((float)column, (float)row);

      bool hMatchRisk = false;
      bool vMatchRisk = false;
      Piece::PieceType hPreviousType;
      Piece::PieceType vPreviousType;

      // Prevent starting the board with matches
      if (column >= 2 && (_gameState->grid[column - 1][row].type 
            == _gameState->grid[column - 2][row].type))
      {
        hPreviousType = _gameState->grid[column - 1][row].type;
        hMatchRisk = true;
      }

      if (row >= 2 && (_gameState->grid[column][row - 1].type 
          == _gameState->grid[column][row - 2].type))
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

//******************************************************************************
// LDK Callbacks
//******************************************************************************

LDKGameSettings gameInit()
{
  LDKGameSettings settings;
  settings.displayWidth = 800;
  settings.displayHeight = 800;
  settings.aspect = 1.0f;
  settings.fullScreen = false;
  settings.name = "LDK jeweld";
  settings.preallocMemorySize = sizeof(GameState);
  return settings;
}

void gameStart(void* memory)
{
  _gameState = (GameState*)memory;

  srand((uint64)memory << 32);

  if (_gameState->initialized)
    return;

  _gameState->initialized = true;

  Vec4 clearColor = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
  renderer::context_initialize(255, clearColor, 0);

  _gameState->material = loadMaterial("./assets/jewel.mat");

  // Calculate matrices and send them to shader uniforms  
  // projection 
  _gameState->projMatrix.orthographic(0, 800, 0, 800, -10, 10);
  renderer::material_setMatrix4(_gameState->material, "mprojection", &_gameState->projMatrix);
  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.translate(Vec3{0, 0, -2});
  renderer::material_setMatrix4(_gameState->material, "mmodel", &_gameState->modelMatrix);

  // Initialize the sprite batch
  _gameState->spriteBatch = renderer::spriteBatch_create(MAX_SPRITES);

  // initialize sprites
  for (uint32 i =0; i < NUM_SPRITES; i++)
  {
    renderer::makeSprite(&_gameState->sprite[i], 
        _gameState->material,
        spriteRect[i].x,
        spriteRect[i].y,
        spriteRect[i].w,
        spriteRect[i].h);
  }

  newGame();
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
    drawGameOver();
    return;
  }

  // gameplay timer
  updateGameTimer(deltaTime);

  // gameplay imput
  if (!_gameState->isSwapping && !_gameState->isAnimating)
  {
    _gameState->isSwapping = updateSwapInput();
  }

  updateMatchedPieces();
  int score = getScore();

  _gameState->isAnimating = updatePieceMovementAnimation(deltaTime, GAME_MOVE_ANIMATION_SPEED)
    || updatePieceMatchAnimation(deltaTime, GAME_SCALE_ANIMATION_SPEED);

  if (!_gameState->isAnimating)
  {
    if (_gameState->isSwapping)
    {
      if (score == 0)
      {
        swap(*_gameState->pieceA, *_gameState->pieceB);
      }

      _gameState->isSwapping = false;
      resetInput();
    }
    else
    {
      updateGrid();
    }

    _gameState->score += score;			
  }

  drawGameplay();
}

void gameStop()
{
  ldk::renderer::material_destroy(_gameState->material);
  ldk::renderer::spriteBatch_destroy(_gameState->spriteBatch);
  ldk::renderer::context_finalize();
}
