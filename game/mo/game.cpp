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
  renderer::Context* context;
  renderer::DrawCall drawCall;
  Mat4 modelMatrix;
  Mat4 projMatrix;
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

  // Initialize material
  _gameState->mesh = ldk::loadMesh("assets/monkey.mesh");
  _gameState->material = ldk::renderer::createMaterial("./assets/standard/test.mat"); //TODO(marcio): make possible to createMaterial(loadMaterial("my_material.mat")). Separate material file loading from material creation
  _gameState->renderable = renderer::createRenderable(_gameState->mesh, _gameState->material);

  _gameState->audio = ldk::loadAudio("assets/crowd.wav");

  // Calculate matrices and send them to shader uniforms  
  // projection
  _gameState->projMatrix.perspective(RADIAN(40), 16/9, 50.0f, -50.0f);
  // model
  _gameState->modelMatrix.identity();
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

  if(axis.x || axis.y || axis.z)
  {
    _gameState->modelMatrix.rotate(axis.x, axis.y, axis.z, RADIAN(80.0f) * deltaTime);
    renderer::setMatrix4(_gameState->material, "mmodel", &_gameState->modelMatrix);
  }
  
  renderer::drawIndexed(_gameState->context, _gameState->renderable);
  renderer::flush(_gameState->context);
}

void gameStop()
{
  ldk::renderer::destroyRenderable(_gameState->renderable);
  ldk::renderer::destroyMaterial(_gameState->material);
  ldk::renderer::destroyContext(_gameState->context);
}

