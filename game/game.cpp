#include <ldk/ldk.h>

ldk::Mesh mesh;

ldk::Mesh makeMesh(ldk::MeshData* meshData)
{
  ldk::Mesh mesh;
  int8* mem = (int8*) meshData;
  mesh.meshData = meshData;
  mesh.indices = (uint32*)(mem + meshData->indicesOffset);
  mesh.vertices = (mem + meshData->verticesOffset);
  return mesh;
}

constexpr uint32 VERTEX_SIZE = 6 * sizeof(float);

using namespace ldk;

struct GameState
{
  uint32 initialized;
  renderer::Sprite sprite;
  renderer::Material material;
  renderer::Texture texture;
  renderer::Context* context;
  renderer::SpriteBatch* spriteBatch;
  renderer::Renderable renderable;
  renderer::VertexBuffer buffer;
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

  // load the meshData
  ldk::MeshData* meshData = ldk::loadMesh("assets/crate.mesh");
  mesh = makeMesh(meshData);

  // Create a vertex buffer
  renderer::makeVertexBuffer(&_gameState->buffer, meshData->vertexCount);
  renderer::addVertexBufferAttribute(&_gameState->buffer, "_pos", 3,
      renderer::VertexAttributeType::FLOAT, 0);

  renderer::addVertexBufferAttribute(&_gameState->buffer, "_normal", 3, 
      renderer::VertexAttributeType::FLOAT, 3 * sizeof(float));

  renderer::addVertexBufferAttribute(&_gameState->buffer, "_uv", 2,
      renderer::VertexAttributeType::FLOAT, 6 * sizeof(float));

  // Initialize material
  renderer::loadMaterial(&_gameState->material, "./assets/standard/unlit_textured.mat");


  // make a renderable 
  uint32 maxIndices = mesh.meshData->indexCount;
  renderer::makeRenderable(&_gameState->renderable, &_gameState->buffer, mesh.indices, maxIndices, true);
  renderer::setMaterial(&_gameState->renderable, &_gameState->material);

  // Calculate matrices and send them to shader uniforms  
  // projection
  _gameState->projMatrix.perspective(RADIAN(40), 16/9, 50.0f, -50.0f);
  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{55.0, 55.0, 55.0});
  _gameState->modelMatrix.translate(Vec3{0, 0, -5});
  
  renderer::setMatrix4(&_gameState->material, "mprojection", &_gameState->projMatrix);
  renderer::setMatrix4(&_gameState->material, "mmodel", &_gameState->modelMatrix);

  // create draw call
  _gameState->drawCall.renderable = &_gameState->renderable;
  _gameState->drawCall.type = renderer::DrawCall::DRAW_INDEXED;
  _gameState->drawCall.vertexCount = mesh.meshData->vertexCount;
  _gameState->drawCall.vertices = (void*)mesh.vertices;
  _gameState->drawCall.indexStart = 0;
  _gameState->drawCall.indexCount = maxIndices;
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

  if(axis.x || axis.y || axis.z)
  {
    _gameState->modelMatrix.rotate(axis.x, axis.y, axis.z, RADIAN(80.0f) * deltaTime);
    renderer::setMatrix4(&_gameState->material, "mmodel", &_gameState->modelMatrix);
  }

  renderer::pushDrawCall(_gameState->context, &_gameState->drawCall);
  renderer::flush(_gameState->context);
}

void gameStop()
{
  //TODO(marcio): How do I destroy a Material ?
  ldk::renderer::destroyContext(_gameState->context);
  ldk::renderer::destroySpriteBatch(_gameState->spriteBatch);
}
