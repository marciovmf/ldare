#include <ldk/ldk.h>

#define STR(s) #s

using namespace ldk;

#define VERTEX_SIZE (6 * sizeof(float))

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

struct GameState
{
  uint32 initialized;
  ldk::gl::Context* context;
  ldk::gl::Shader shader;
  ldk::gl::Renderable renderable;
  ldk::gl::VertexBuffer buffer;
  ldk::gl::DrawCall drawCall;
  ldk::Mat4 modelMatrix;
  ldk::Mat4 projMatrix;
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

static GameState* _gameState;

// Vertex shader
char* vs = STR(#version 330\n
	in vec3 _pos; 
	in vec3 _color; 
	out vec3 fragColor;
  uniform mat4 mmodel;
  uniform mat4 mprojection;
  void main()
  {
	  gl_Position = mprojection * mmodel * vec4(_pos, 1.0); \n
	  fragColor = _color;
  });

// Fragment shader
char* fs = STR(#version 330\n
  in vec3 fragColor;
  out vec4 out_color;
  void main()
  {
	  out_color = vec4(fragColor, 1.0);
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

  _gameState->context = ldk::gl::createContext(255, GL_COLOR_BUFFER_BIT ,0);
  ldk::gl::makeVertexBuffer(&_gameState->buffer, 64, VERTEX_SIZE);
  ldk::gl::addVertexBufferAttribute(&_gameState->buffer, "_pos", 3, ldk::gl::VertexAttributeType::FLOAT, 0);
  ldk::gl::addVertexBufferAttribute(&_gameState->buffer, "_color", 3, ldk::gl::VertexAttributeType::FLOAT,  3 * sizeof(float));
  ldk::gl::loadShader(&_gameState->shader, vs, fs);

  uint32 maxIndices = (sizeof(indices) / sizeof (uint32));
  ldk::gl::makeRenderable(&_gameState->renderable, &_gameState->buffer, indices, maxIndices, true);
  ldk::gl::setShader(&_gameState->renderable, &_gameState->shader);

  // compose draw call
  _gameState->drawCall.renderable = &_gameState->renderable;
  _gameState->drawCall.type = gl::DrawCall::DRAW_INDEXED;
  _gameState->drawCall.textureCount = 0;
  _gameState->drawCall.vertexCount = 36;
  _gameState->drawCall.vertices = mesh;
  _gameState->drawCall.indexStart = 0;
  _gameState->drawCall.indexCount = maxIndices;
  _gameState->initialized = 1;

  // projection
  _gameState->projMatrix.perspective(RADIAN(45), 4/3, 50.0f, -50.0f);
  ldk::gl::setShaderParam(&_gameState->shader, "mprojection", &_gameState->projMatrix);

  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{10.0, 10.0, 10.0});
  _gameState->modelMatrix.translate(Vec3{0, 0, -10});
  ldk::gl::setShaderParam(&_gameState->shader, "mmodel", &_gameState->modelMatrix);
}

void gameUpdate(float deltaTime) 
{
#if 1
  Vec3 axis = {};
  if (input::getKey(LDK_KEY_J))
  {
    axis.x = 1;
  }
  else if (input::getKey(LDK_KEY_K))
  {
    axis.y = 1;
  }
  else if (input::getKey(LDK_KEY_L))
  {
    axis.z = 1;
  }

  if(axis.x || axis.y || axis.z)
  {
    _gameState->modelMatrix.rotate(axis.x, axis.y, axis.z, RADIAN(35.0f) * deltaTime);
    ldk::gl::setShaderParam(&_gameState->shader, "mmodel", &_gameState->modelMatrix);
  }
#else
    _gameState->modelMatrix.rotate(1,1,1, RADIAN(45.0f) * deltaTime);
    ldk::gl::setShaderParam(&_gameState->shader, "mmodel", &_gameState->modelMatrix);
#endif

  ldk::gl::pushDrawCall(_gameState->context, &_gameState->drawCall);
  ldk::gl::flush(_gameState->context);
}

void gameStop()
{
  ldk::gl::destroyContext(_gameState->context);
}
