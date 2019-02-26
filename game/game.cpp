#include <ldk/ldk.h>

float mesh[]
{   
    // vertex color
    // front
    -1.0, -1.0,  1.0,   1.0, 0.0, 0.0,
     1.0, -1.0,  1.0,   0.0, 1.0, 0.0,
     1.0,  1.0,  1.0,   0.0, 0.0, 1.0,
    -1.0,  1.0,  1.0,   1.0, 1.0, 1.0,
    // back
    -1.0, -1.0, -1.0,   1.0, 0.0, 0.0,
     1.0, -1.0, -1.0,   0.0, 1.0, 0.0,
     1.0,  1.0, -1.0,   0.0, 0.0, 1.0,
    -1.0,  1.0, -1.0,   1.0, 1.0, 1.0,
};

uint32 indices[] = 
{
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3,
	};

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

  // Create a vertex buffer
  renderer::makeVertexBuffer(&_gameState->buffer, 64);
  renderer::addVertexBufferAttribute(&_gameState->buffer, "_pos", 3, renderer::VertexAttributeType::FLOAT, 0);
  renderer::addVertexBufferAttribute(&_gameState->buffer, "_color", 3, renderer::VertexAttributeType::FLOAT,  3 * sizeof(float));

  // Initialize material
  renderer::loadMaterial(&_gameState->material, "./assets/standard/unlit_textured.mat");
  
  // make a renderable 
  uint32 maxIndices = (sizeof(indices) / sizeof (uint32));
  renderer::makeRenderable(&_gameState->renderable, &_gameState->buffer, indices, maxIndices, true);
  renderer::setMaterial(&_gameState->renderable, &_gameState->material);

  // Calculate matrices and send them to shader uniforms  
  // projection
  _gameState->projMatrix.perspective(RADIAN(45), 4/3, 50.0f, -50.0f);
  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{10.0, 10.0, 10.0});

  _gameState->modelMatrix.translate(Vec3{0, 0, -10});
  
  renderer::setMatrix4(&_gameState->material, "mprojection", &_gameState->projMatrix);
  renderer::setMatrix4(&_gameState->material, "mmodel", &_gameState->modelMatrix);

  // create draw call
  _gameState->drawCall.renderable = &_gameState->renderable;
  _gameState->drawCall.type = renderer::DrawCall::DRAW_INDEXED;
  _gameState->drawCall.vertexCount = 36;
  _gameState->drawCall.vertices = mesh;
  _gameState->drawCall.indexStart = 0;
  _gameState->drawCall.indexCount = maxIndices;
  _gameState->initialized = 1;
}

void gameUpdate(float deltaTime) 
{
  Vec3 axis = {};
  if (input::getKey(ldk::input::LDK_KEY_J))
  {
    axis.x = 1;
  }
  else if (input::getKey(ldk::input::LDK_KEY_K))
  {
    axis.y = 1;
  }
  else if (input::getKey(ldk::input::LDK_KEY_L))
  {
    axis.z = 1;
  }

  if(axis.x || axis.y || axis.z)
  {
    _gameState->modelMatrix.rotate(axis.x, axis.y, axis.z, RADIAN(35.0f) * deltaTime);
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
