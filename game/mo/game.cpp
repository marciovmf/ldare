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
  Handle renderable2;
  Handle font;
  Handle fontMaterial;
  renderer::Context* context;
  renderer::SpriteBatch* spriteBatch;
  Mat4 modelMatrix;
  Mat4 modelMatrix2;
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
  _gameState->renderable2 = renderer::createRenderable(_gameState->mesh, _gameState->material);

  _gameState->audio = ldk::loadAudio("assets/crowd.wav");

  // Calculate matrices and send them to shader uniforms  
  // projection matrix
  _gameState->projMatrix.perspective(RADIAN(40), 16/9, 50.0f, -50.0f);
  renderer::setMatrix4(_gameState->material, "mprojection", &_gameState->projMatrix);
  
  // model matrices
  // mesh 1
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{55.0, 55.0, 55.0});
  _gameState->modelMatrix.translate(Vec3{1, 0, -5});
  // mesh 2 
  _gameState->modelMatrix2.identity();
  _gameState->modelMatrix2.scale(Vec3{30.0, 30.0, 30.0});
  _gameState->modelMatrix2.translate(Vec3{-1, 0, -5});

  _gameState->initialized = 1;
}

void gameUpdate(float deltaTime) 
{
  bool update = false;
  Vec3 axis = {};

  // draw mesh 1
  renderer::setMatrix4(_gameState->material, "mprojection", &_gameState->projMatrix);
  renderer::setMatrix4(_gameState->material, "mmodel", &_gameState->modelMatrix);
  renderer::drawIndexed(_gameState->context, _gameState->renderable);

  // draw mesh 2
  renderer::setMatrix4(_gameState->material, "mprojection", &_gameState->projMatrix);
  renderer::setMatrix4(_gameState->material, "mmodel", &_gameState->modelMatrix2);
  renderer::drawIndexed(_gameState->context, _gameState->renderable2);

  renderer::flush(_gameState->context);
}

void gameStop()
{
  ldk::renderer::destroyRenderable(_gameState->renderable);
  ldk::renderer::destroyMaterial(_gameState->material);
  ldk::renderer::destroyContext(_gameState->context);
}

