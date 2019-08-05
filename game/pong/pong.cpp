
#include <ldk/ldk.h>
using namespace ldk;

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

// Global game state variables
static struct GameState
{
  // rendering stuff
  uint32 initialized;
  HMaterial material;
  Mat4 projMatrix;
  renderer::Sprite sprite;
  Rect viewPort;

  // paddle dimentions
  Rect paddleLeft;
  Rect paddleRight;

  Rect ball;
  Vec2 ballDirection;

  // limits
  float paddleSpeed;
  float ballSpeed;
  float maxPaddleY;

  // new ball delay
  float waitDelay;

} *_gameState;

//******************************************************************************
// LDK Callbacks
//******************************************************************************
LDKGameSettings gameInit()
{
  LDKGameSettings settings;
  settings.displayWidth = DEFAULT_WINDOW_WIDTH;
  settings.displayHeight = DEFAULT_WINDOW_HEIGHT;
  settings.aspect = 1.0f;
  settings.showCursor = false;
  settings.fullScreen = false;
  settings.name = "LDK pong";
  settings.preallocMemorySize = sizeof(GameState);
  return settings;
}

void gameStart(void* memory)
{
  _gameState = (GameState*)memory;

  if (_gameState->initialized)
    return;

  _gameState->initialized = true;

  Vec4 clearColor = Vec4{0.0f, 0.0f, 0.0f, 0.0f};

  //TODO(marcio): Size in windowed mode is wrong. It is not subtracting window title bar height!
  renderer::context_initialize(255, clearColor, 0); 

  _gameState->material = loadMaterial("./assets/pong.mat");

  // Calculate matrices and send them to shader uniforms  
  // projection 
  _gameState->projMatrix.orthographic(0, DEFAULT_WINDOW_WIDTH, 0, DEFAULT_WINDOW_HEIGHT, -10, 10);

  // Initialize the sprite batch
  renderer::spriteBatch_initialize(5);

  // initialize sprites
  renderer::makeSprite(&_gameState->sprite, _gameState->material,0,0,1,1);

  _gameState->ballDirection.x = 0.6f;
  _gameState->ballDirection.y = 0.6f;
  _gameState->ball.x = _gameState->viewPort.w/2.0f;
  _gameState->ball.y = _gameState->viewPort.h/2.0f;
}

void gameViewResized(uint32 width, uint32 height)
{
  _gameState->viewPort = {0.0f, 0.0f, (float) width, (float) height};
  renderer::setViewPort(_gameState->viewPort);

  // update view matrix
  _gameState->projMatrix.orthographic(0, width, 0, height, -10, 10);

  // update dimentions
  float margin = (_gameState->viewPort.w * 0.05f); 
  float paddleHeight = _gameState->viewPort.h * 0.2f;
  float paddleWidth = _gameState->viewPort.w * 0.015f;

  _gameState->paddleLeft.x = margin;
  _gameState->paddleLeft.w = paddleWidth;
  _gameState->paddleLeft.h = paddleHeight;

  _gameState->paddleRight.x = _gameState->viewPort.w - paddleWidth - margin;
  _gameState->paddleRight.w = paddleWidth;
  _gameState->paddleRight.h = paddleHeight;

  _gameState->ball.w = paddleWidth;
  _gameState->ball.h = paddleWidth;

  // limits
  _gameState->paddleSpeed = _gameState->viewPort.h * 0.9f;
  _gameState->ballSpeed = _gameState->viewPort.w/2.0f;
  _gameState->maxPaddleY = _gameState->viewPort.h - paddleHeight;
}

void draw(float deltaTime)
{
  renderer::clearBuffers(renderer::Context::COLOR_BUFFER | renderer::Context::DEPTH_BUFFER);
  renderer::beginFrame(_gameState->projMatrix);
  renderer::spriteBatch_begin();

  const Rect& paddleLeft = _gameState->paddleLeft;
  const Rect& paddleRight = _gameState->paddleRight;
  const Rect& ball = _gameState->ball;

  renderer::spriteBatch_draw(&_gameState->sprite,
      paddleLeft.x,
      paddleLeft.y,
      paddleLeft.w,
      paddleLeft.h);

  renderer::spriteBatch_draw(&_gameState->sprite,
      paddleRight.x,
      paddleRight.y,
      paddleRight.w,
      paddleRight.h);

  renderer::spriteBatch_draw(&_gameState->sprite,
      ball.x,
      ball.y,
      ball.w,
      ball.h);

  renderer::spriteBatch_end();
  renderer::endFrame();
}

void gameUpdate(float deltaTime)
{
  float speed = _gameState->paddleSpeed * deltaTime;
  Rect& paddleLeft = _gameState->paddleLeft;
  Rect& paddleRight = _gameState->paddleRight;
  Rect& ball = _gameState->ball;
  Rect& viewPort = _gameState->viewPort;
  Vec2& ballDirection = _gameState->ballDirection;

  // one third of piece height area that triggers a direction change
  float thirdPiece = paddleLeft.h * 0.3f;

  // Paddles movement
  if (input::getKey(input::LDK_KEY_W))
  {
    paddleLeft.y = MIN(paddleLeft.y + speed, _gameState->maxPaddleY);
    paddleRight.y = MIN(paddleRight.y + speed, _gameState->maxPaddleY);
  }
  else if (input::getKey(input::LDK_KEY_S))
  {
    paddleLeft.y = MAX(paddleLeft.y - speed, 0);
    paddleRight.y = MAX(paddleRight.y - speed, 0);
  }

  if (_gameState->waitDelay > 0)
  {
    _gameState->waitDelay -= deltaTime;
    draw(deltaTime);
    return;
  }

  // Left paddle collision
  ball.x += ballDirection.x * _gameState->ballSpeed * deltaTime;
  ball.y += ballDirection.y * _gameState->ballSpeed * deltaTime;
  float ballCenter = ball.y + ball.h * 0.5f;

  if (ball.x <= paddleLeft.x + paddleLeft.w 
      && ball.x + ball.w >= paddleLeft.x + paddleLeft.w
      && ball.y + ball.h > paddleLeft.y
      && ball.y < paddleLeft.y + paddleLeft.h)
  {

    ball.x = paddleLeft.x + paddleLeft.w + 0.1f;
   
    // change direction ?
    if(ballCenter > paddleLeft.y + thirdPiece
        || ballCenter < paddleLeft.y + paddleLeft.h - thirdPiece)
      ballDirection.y *= -1.0;

    ballDirection.x *= -1.0;
  }
  // Right paddle collision
  else if (ball.x + ball.w >= paddleRight.x
      && ball.x <= paddleRight.x
      && ball.y + ball.h > paddleRight.y
      && ball.y < paddleRight.y + paddleRight.h)
  {
    ball.x = paddleRight.x - paddleLeft.w - 0.1f;

    // change direction ?
    if(ballCenter > paddleRight.y + thirdPiece
        || ballCenter < paddleRight.y + paddleRight.h - thirdPiece)
      ballDirection.y *= -1.0;

    ballDirection.x *= -1;
  }
  else
  {
    // Vertical limits
    if (ball.y + ball.w > viewPort.h || ball.y <= 0)
      _gameState->ballDirection.y *= -1;

    // Horizontal limits
    if (ball.x + ball.w > viewPort.w || ball.x <= 0)
    {
      ball.x = viewPort.w * 0.5f;
      ball.y = viewPort.h * 0.5f;
      ballDirection.x *= -1;
      _gameState->waitDelay = 1.0f;
    }
  }

  draw(deltaTime);
}

void gameStop()
{
  ldk::renderer::material_destroy(_gameState->material);
  ldk::renderer::spriteBatch_finalize();
  ldk::renderer::context_finalize();
}

