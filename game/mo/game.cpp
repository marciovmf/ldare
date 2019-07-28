#include <ldk/ldk.h>

using namespace ldk;

struct GameState
{
  uint32 initialized;
  renderer::Sprite sprite;
  HMaterial material;
  HMaterial fontMaterial;
  HMesh mesh;
  HAudio audio;
  HRenderable renderable;
  HRenderable renderable2;
  HFont font;
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

  Vec4 clearColor = Vec4{0.0f, 1.0f, 1.0f, 0.0f};

  renderer::context_initialize(255, clearColor, 0);
  renderer::context_setClearColor(clearColor);
  _gameState->spriteBatch = ldk::renderer::spriteBatch_create(1024);

  // load materials, font and audio
  _gameState->mesh = ldk::asset_loadMesh("assets/monkey.mesh");
  _gameState->material = ldk::loadMaterial("./assets/standard/test.mat"); 
  _gameState->fontMaterial = ldk::loadMaterial("./assets/standard/Inconsolata_18.mat"); 
  
  _gameState->font = ldk::asset_loadFont("./assets/standard/Inconsolata_18.font"); 
  _gameState->audio = ldk::asset_loadAudio("assets/crowd.wav");

  // create renderables
  _gameState->renderable = renderer::renderable_create(_gameState->mesh, _gameState->material);
  _gameState->renderable2 = renderer::renderable_create(_gameState->mesh, _gameState->material);

  // Calculate matrices and send them to shader uniforms  
  // projection matrix
  _gameState->projMatrix.perspective(RADIAN(40), 16/9, 50.0f, -50.0f);
  renderer::material_setMatrix4(_gameState->material, "mprojection", &_gameState->projMatrix);
  
  // model matrices
  // mesh 1
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{55.0, 55.0, 55.0});
  _gameState->modelMatrix.translate(Vec3{1, 0, -5});
  renderer::renderable_setMatrix(_gameState->renderable, &_gameState->modelMatrix);
  // mesh 2 
  _gameState->modelMatrix2.identity();
  _gameState->modelMatrix2.scale(Vec3{30.0, 30.0, 30.0});
  _gameState->modelMatrix2.translate(Vec3{-1, 0, -5});
  renderer::renderable_setMatrix(_gameState->renderable2, &_gameState->modelMatrix2);

  _gameState->initialized = 1;
}

void gameUpdate(float deltaTime) 
{
  if (input::isKeyDown(input::LDK_KEY_SPACE))
  {
    playAudio(_gameState->audio);
  }

  renderer::clearBuffers(renderer::Context::COLOR_BUFFER | renderer::Context::DEPTH_BUFFER);
  renderer::drawIndexed(_gameState->renderable);
  renderer::drawIndexed(_gameState->renderable2);
  renderer::flush();
}

void gameStop()
{
  ldk::asset_unload(_gameState->audio);
  ldk::asset_unload(_gameState->font);
  ldk::asset_unload(_gameState->mesh);
  ldk::renderer::material_destroy(_gameState->material);
  ldk::renderer::material_destroy(_gameState->fontMaterial);
  ldk::renderer::renderable_destroy(_gameState->renderable);
  ldk::renderer::renderable_destroy(_gameState->renderable2);
  ldk::renderer::context_finalize();
}

