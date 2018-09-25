#include <ldk/ldk.h>

#define STR(s) #s

using namespace ldk;

float mesh[] = {
  -0.5f,	-0.5f,	0.0f, 1.0f, 0.0f, 0.0f,
  0.5f,		-0.5f,	0.0f, 0.0f, 1.0f, 0.0f,
  0.0f,		0.5f,	0.0f, 0.0f, 0.0f, 1.0f,
};

#define VERTEX_SIZE (6 * sizeof(float))

struct GameState
{
  uint32 initialized;
  ldk::gl::Context* context;
  ldk::gl::Shader shader;
  ldk::gl::Renderable renderable;
  ldk::gl::VertexBuffer buffer;
  ldk::gl::DrawCall drawCall;
};

static GameState* _gameState;

// Vertex shader
char* vs = STR(#version 330\n
	in vec3 _pos; \n
	in vec3 _color; \n
	out vec3 fragColor;\n
  void main()\n
  {
	gl_Position = vec4(_pos, 1.0); \n
	fragColor = _color;
  });

// Fragment shader
char* fs = STR(#version 330\n
  in vec3 fragColor; \n
  out vec4 out_color;\n
  
  void main()\n
  {
	  out_color = vec4(fragColor, 1.0); \n
  });

size_t gameInit()
{
  return sizeof(GameState);
}

void gameStart(void* memory)
{
  _gameState = (GameState*)memory;

  if (_gameState->initialized)
    return;

  _gameState->context = ldk::gl::createContext(255, GL_COLOR_BUFFER_BIT,0);
  
  ldk::gl::makeVertexBuffer(&_gameState->buffer, 3, VERTEX_SIZE);
  ldk::gl::addVertexBufferAttribute(&_gameState->buffer, "_pos", 3, ldk::gl::VertexAttributeType::FLOAT, 0);
  ldk::gl::addVertexBufferAttribute(&_gameState->buffer, "_color", 3, ldk::gl::VertexAttributeType::FLOAT,  3 * sizeof(float));

  ldk::gl::loadShader(&_gameState->shader, vs, fs);
  ldk::gl::makeRenderable(&_gameState->renderable, &_gameState->buffer, true);

  ldk::gl::setShader(&_gameState->renderable, &_gameState->shader);

  // compose draw call
  _gameState->drawCall.renderable = &_gameState->renderable;
  _gameState->drawCall.textureCount = 0;
  _gameState->drawCall.vertexCount = 3;
  _gameState->drawCall.vertices = mesh;
  _gameState->initialized;
}

void gameUpdate(float deltaTime) 
{
  ldk::gl::pushDrawCall(_gameState->context, &_gameState->drawCall);
  ldk::gl::flush(_gameState->context);
}

void gameStop()
{
  ldk::gl::destroyContext(_gameState->context);
}
