#include <ldk/ldk.h>

using namespace ldk;

struct GameState
{
  uint32 initialized;
  renderer::Sprite sprite;
  Handle material;
  Handle mesh;
  Handle audio;
  Handle renderable;
  Handle font;
  Handle fontMaterial;
  renderer::Context* context;
  renderer::SpriteBatch* spriteBatch;
  Mat4 modelMatrix;
  Mat4 modelMatrixText;
  Mat4 projMatrix;
  Mat4 projMatrixText;
};

static GameState* _gameState;

LDKGameSettings gameInit()
{
  LDKGameSettings settings = ldk::loadGameSettings();
  settings.preallocMemorySize = sizeof(GameState);
  return settings;
}

void gameStart(void* memory)
{
  _gameState = (GameState*)memory;

  if (_gameState->initialized)
    return;

  _gameState->context =
  renderer::createContext(255, renderer::Context::COLOR_BUFFER | renderer::Context::DEPTH_BUFFER, 0);
  _gameState->spriteBatch = ldk::renderer::spriteBatch_create(_gameState->context, 1024);

  // Initialize material
  _gameState->mesh = ldk::loadMesh("assets/monkey.mesh");
  _gameState->material = ldk::renderer::createMaterial("./assets/standard/test.mat"); 
  _gameState->fontMaterial = ldk::renderer::createMaterial("./assets/standard/Inconsolata_18.mat"); 
  _gameState->font = ldk::loadFont("./assets/standard/Inconsolata_18.font"); 
  _gameState->renderable = renderer::createRenderable(_gameState->mesh, _gameState->material);

  _gameState->audio = ldk::loadAudio("assets/crowd.wav");

  // Calculate matrices and send them to shader uniforms  
  // projection
  _gameState->projMatrix.perspective(RADIAN(40), 16/9, 50.0f, -50.0f);
  
  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrixText.identity();

  // 2d Projection matrix
  _gameState->projMatrixText.orthographic(0, 800, 0, 800, -10, 10);
  renderer::setMatrix4(_gameState->fontMaterial, "mprojection", &_gameState->projMatrixText);
  renderer::setMatrix4(_gameState->fontMaterial, "mmodel", &_gameState->modelMatrixText);

  // 3d projection matrix
  _gameState->modelMatrix.scale(Vec3{55.0, 55.0, 55.0});
  _gameState->modelMatrix.translate(Vec3{0, 0, -5});
  renderer::setMatrix4(_gameState->material, "mprojection", &_gameState->projMatrix);
  renderer::setMatrix4(_gameState->material, "mmodel", &_gameState->modelMatrix);

  _gameState->initialized = 1;
}

void gameUpdate(float deltaTime) 
{
  bool update = false;
  Vec3 axis = {};

  if (input::getKey(ldk::input::LDK_KEY_H))
  {
    axis.y = 1;
  }
  else if (input::getKey(ldk::input::LDK_KEY_L))
  {
    axis.y = -1;
  }
  else if (input::getKey(ldk::input::LDK_KEY_J))
  {
    axis.x = 1;
  }
  else if (input::getKey(ldk::input::LDK_KEY_K))
  {
    axis.x = -1;
  }

  if(input::isKeyDown(ldk::input::LDK_KEY_SPACE))
  {
    ldk::playAudio(_gameState->audio);
  }

  renderer::setMatrix4(_gameState->material, "mprojection", &_gameState->projMatrix);
  if(axis.x || axis.y || axis.z)
  {
    _gameState->modelMatrix.rotate(axis.x, axis.y, axis.z, RADIAN(80.0f) * deltaTime);
    renderer::setMatrix4(_gameState->material, "mmodel", &_gameState->modelMatrix);
  }
  
  renderer::drawIndexed(_gameState->context, _gameState->renderable);

  Vec3 pos = {100.0f, 150.0f, 0.0f};
  Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

  // Text/2D rendering
  //renderer::setMatrix4(_gameState->fontMaterial, "mprojection", &_gameState->projMatrixText);
  //renderer::setMatrix4(_gameState->fontMaterial, "mmodel", &_gameState->modelMatrixText);
  //renderer::spriteBatch_begin(_gameState->spriteBatch);
  //renderer::spriteBatch_drawText(_gameState->spriteBatch,
  //    _gameState->fontMaterial,
  //    _gameState->font,
  //    pos,
  //    "Hello world!",
  //    1.0f,
  //    color);
  //renderer::spriteBatch_end(_gameState->spriteBatch);
  //renderer::flush(_gameState->context);
}

void gameStop()
{
  ldk::renderer::destroyRenderable(_gameState->renderable);
  ldk::renderer::destroyMaterial(_gameState->material);
  ldk::renderer::destroyContext(_gameState->context);
}

