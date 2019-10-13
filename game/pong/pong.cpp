
#include <ldk/ldk.h>
#include <ldkEngine/ldk_memory.h>
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

  // text rendering
  HFont font;
  HMaterial fontMaterial;

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

  Vec2 cursor;

  bool showMsg;
  char msg[256];
  int32 msgSize;

} *_gameState;

//******************************************************************************
// LDK Callbacks
//******************************************************************************
LDKGameSettings onInit()
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

void gameViewResized(uint32 width, uint32 height);

void onStart(void* memory)
{
  _gameState = (GameState*)memory;

  if (_gameState->initialized)
    return;

  _gameState->initialized = true;

  Vec4 clearColor = Vec4{0.0f, 0.0f, 0.0f, 0.0f};

  //TODO(marcio): Size in windowed mode is wrong. It is not subtracting window title bar height!
  renderer::context_initialize(5, clearColor, 0); 

  _gameState->material = loadMaterial("./assets/pong.mat");

  _gameState->fontMaterial = ldk::loadMaterial("./assets/standard/Inconsolata_18.mat"); 
  _gameState->font = ldk::asset_loadFont("./assets/standard/Inconsolata_18.font"); 

  // Calculate matrices and send them to shader uniforms  
  // projection 
  _gameState->projMatrix.orthographic(0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, -10, 10);

  // Initialize the sprite batch
  renderer::spriteBatch_initialize(1024);

  // initialize sprites
  renderer::makeSprite(&_gameState->sprite, _gameState->material, 0, 0, 1, 1);

  _gameState->ballDirection.x = 0.6f;
  _gameState->ballDirection.y = 0.6f;
  _gameState->ball.x = _gameState->viewPort.w/2.0f;
  _gameState->ball.y = _gameState->viewPort.h/2.0f;

  gameViewResized(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  _gameState->showMsg = false;
}

void gameViewResized(uint32 width, uint32 height)
{
  _gameState->viewPort = {0.0f, 0.0f, (float) width, (float) height};
  renderer::setViewPort(_gameState->viewPort);

  // update view matrix
  _gameState->projMatrix.orthographic(0, width, height, 0, -10, 10);

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

float memReportRefresh = 0.0f;
char* memReport;

void draw(float deltaTime)
{
  memReportRefresh -= deltaTime;
  if (memReportRefresh <= 0.0f)
  {
    memReport = (char*) ldkEngine::memory_getReport();
    memReportRefresh = 5.0f;
  }

  Vec4 textColor = Vec4{1.0f, 1.0f, 1.0f, 1.0f};
  Vec4 bgColor = {.1f, .1f, .1f, 1.0f};
  Vec3 textPos = Vec3{10, 10, 0.0f};

  const Rect& paddleLeft = _gameState->paddleLeft;
  const Rect& paddleRight = _gameState->paddleRight;
  const Rect& ball = _gameState->ball;
  
  Vec4 color = Vec4{ball.x / 800, ball.y / 600, paddleLeft.y, 1.0f};

  renderer::clearBuffers(renderer::COLOR_BUFFER | renderer::DEPTH_BUFFER);
  renderer::beginFrame(_gameState->projMatrix);
  renderer::spriteBatch_begin();

  const Vec2& mousePos = _gameState->cursor;

  // mouse cursor
  renderer::spriteBatch_draw(&_gameState->sprite, mousePos.x, mousePos.y, 15, 15, color);

  // text
  if(_gameState->showMsg)
  {
    Vec2& textSize = renderer::spriteBatch_drawText(_gameState->fontMaterial,
        _gameState->font, textPos, _gameState->msg, 1.0f, textColor);

    if (mousePos.x >= textPos.x && mousePos.x <= textPos.x + textSize.x
        && mousePos.y >= textPos.y && mousePos.y <= textPos.y + textSize.y)
      bgColor = {.4f, .1f, .4f, 1.0f};

    renderer::spriteBatch_draw(&_gameState->sprite, textPos.x, textPos.y, textSize.x, textSize.y, bgColor);
  }

  renderer::spriteBatch_draw(&_gameState->sprite, paddleLeft.x, paddleLeft.y,
      paddleLeft.w, paddleLeft.h, color);
  
  renderer::spriteBatch_draw(&_gameState->sprite, paddleRight.x, paddleRight.y,
      paddleRight.w, paddleRight.h, color);

  renderer::spriteBatch_draw(&_gameState->sprite, ball.x, ball.y, ball.w, ball.h, color);



  renderer::spriteBatch_end();
  renderer::endFrame();
}

void onUpdate(float deltaTime)
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
  if (input::getKey(input::LDK_KEY_S))
  {
    paddleLeft.y = MIN(paddleLeft.y + speed, _gameState->maxPaddleY);
    paddleRight.y = MIN(paddleRight.y + speed, _gameState->maxPaddleY);
  }
  else if (input::getKey(input::LDK_KEY_W))
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

bool onEvent(const ldk::Event* event)
{

  if(event->type == ldk::EventType::MOUSE_MOVE_EVENT)
  {
    _gameState->cursor.x = event->mouseMoveEvent.x;
    _gameState->cursor.y = event->mouseMoveEvent.y;
    return true;
  }

  if(event->type == ldk::EventType::VIEW_EVENT 
    && event->viewEvent.type == ldk::ViewEvent::VIEW_RESIZED)
  {
    gameViewResized(event->viewEvent.width, event->viewEvent.height);
    return true;
  }

  if(event->type == ldk::EventType::KEYBOARD_EVENT 
    && event->keyboardEvent.type == ldk::KeyboardEvent::KEYBOARD_KEY_DOWN)
  {
      if(event->keyboardEvent.key == ldk::input::LDK_KEY_TAB &&
          event->keyboardEvent.isControlDown)
      {
        _gameState->showMsg = !_gameState->showMsg;
        return true;
      }
  }

  if (_gameState->showMsg)
  {
    if(event->type == ldk::EventType::TEXT_INPUT_EVENT)
    {
      int32& msgSize = _gameState->msgSize;

      if(event->textInputEvent.key == ldk::input::LDK_KEY_BACK)
      {
        msgSize--;
        if(msgSize < 0) msgSize = 0;
        _gameState->msg[msgSize] = 0;
      }
      else
      {
        if(event->textInputEvent.key == ldk::input::LDK_KEY_RETURN)
        {
          _gameState->msg[msgSize] = '\n';
        }
        else
        {
          _gameState->msg[msgSize] = (char)event->textInputEvent.text;
        }
        _gameState->msg[++msgSize] = 0;
        if(_gameState->msgSize > 255)
          msgSize = 0;
      }

      return true;
    }
  }
  return false;
}

void onStop()
{
  ldk::asset_unload(_gameState->font);
  ldk::renderer::material_destroy(_gameState->fontMaterial);
  ldk::renderer::material_destroy(_gameState->material);
  ldk::renderer::spriteBatch_finalize();
  ldk::renderer::context_finalize();
}

