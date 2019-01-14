#include <ldk/ldk.h>

using namespace ldk;

float mesh[]
{   
    // Coord    UV
    0.0, 0.0, 0.0, 0.0, 0.0,
    2.0, 0.0, 0.0, 1.0, 0.0,
    2.0, 2.0, 0.0, 1.0, 1.0,
    0.0, 2.0, 0.0, 0.0, 1.0
};

#define VERTEX_SIZE (5 * sizeof(float))

uint32 indices[] = 
{
		0, 1, 2,
    2, 3, 0
};

struct GameState
{
  uint32 initialized;
  ldk::renderer::Material material;
  ldk::renderer::Texture textureId;
  ldk::renderer::Context* context;
  ldk::renderer::Renderable renderable;
  ldk::renderer::VertexBuffer buffer;
  ldk::renderer::DrawCall drawCall;
  ldk::Mat4 modelMatrix;
  ldk::Mat4 projMatrix;
};

static GameState* _gameState;

#define STR(s) #s
// Vertex shader
char* vs = STR(#version 330 core\n
	in vec3 _pos; 
	in vec2 _uuv; 
  out vec2 fragCoord;
  uniform mat4 mmodel;
  uniform mat4 mprojection;
  void main()
  {
	  gl_Position = mprojection * mmodel * vec4(_pos, 1.0); \n
    fragCoord = _uuv;
  });

// Fragment shader
char* fs = STR(#version 330 core\n
  in vec2 fragCoord;
  uniform sampler2D _mainTexture;
  out vec4 out_color;

  void main()
  {
    vec4 solidColor = vec4(1.0, 1.0, 1.0, 1.0); 

    vec4 textureColor = texture(_mainTexture, fragCoord);
    out_color = mix(solidColor, textureColor, 0.9);
  });


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
    ldk::renderer::createContext(255, ldk::renderer::Context::COLOR_BUFFER | ldk::renderer::Context::DEPTH_BUFFER, 0);

  // Make vertex buffer
  ldk::renderer::makeVertexBuffer(&_gameState->buffer, 64);
  ldk::renderer::addVertexBufferAttribute(&_gameState->buffer, "_pos", 3, ldk::renderer::VertexAttributeType::FLOAT, 0);
  ldk::renderer::addVertexBufferAttribute(&_gameState->buffer, "_uuv", 2, ldk::renderer::VertexAttributeType::FLOAT,  3 * sizeof(float));

  // Load Textures from bmp
  auto bmp = ldk::loadBitmap("Assets/test.bmp");
  _gameState->textureId = ldk::renderer::createTexture(bmp);
  ldk::freeAsset((void*) bmp);

  // Initialize material
  makeMaterial(&_gameState->material, vs, fs);
  _gameState->material.textureCount = 1;
  _gameState->material.texture[0] = _gameState->textureId;

  // create a renderable and set material
  uint32 maxIndices = (sizeof(indices) / sizeof (uint32));
  ldk::renderer::makeRenderable(&_gameState->renderable, &_gameState->buffer, indices, maxIndices, true);
  ldk::renderer::setMaterial(&_gameState->renderable, &_gameState->material);

  // compose draw call
  _gameState->drawCall.renderable = &_gameState->renderable;
  _gameState->drawCall.type = renderer::DrawCall::DRAW_INDEXED;
  _gameState->drawCall.vertexCount = sizeof(mesh)/VERTEX_SIZE;
  _gameState->drawCall.vertices = mesh;
  _gameState->drawCall.indexStart = 0;
  _gameState->drawCall.indexCount = maxIndices;
  _gameState->initialized = 1;

  // set shader attributes:
  // assign to texture slot 0
  ldk::renderer::setShaderInt(&_gameState->material.shader, "_mainTexture", 0); 

  // projection 
  _gameState->projMatrix.perspective(RADIAN(45), 4/3, 50.0f, -50.0f);
  ldk::renderer::setShaderMatrix4(&_gameState->material.shader, "mprojection", &_gameState->projMatrix);

  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{10.0, 10.0, 10.0});
  _gameState->modelMatrix.translate(Vec3{0, 0, -10});
  ldk::renderer::setShaderMatrix4(&_gameState->material.shader, "mmodel", &_gameState->modelMatrix);
}

void gameUpdate(float deltaTime) 
{
#if 1
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
    ldk::renderer::setShaderMatrix4(&_gameState->material.shader, "mmodel", &_gameState->modelMatrix);
  }
#else
    _gameState->modelMatrix.rotate(1,1,1, RADIAN(45.0f) * deltaTime);
    ldk::renderer::setShaderMatrix4(&_gameState->shader, "mmodel", &_gameState->modelMatrix);
#endif

  ldk::renderer::pushDrawCall(_gameState->context, &_gameState->drawCall);
  ldk::renderer::flush(_gameState->context);
}

void gameStop()
{
  ldk::renderer::destroyTexture(_gameState->textureId);
  ldk::renderer::destroyContext(_gameState->context);
}
