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
  ldk::Bitmap* bmpTexture;
  ldk::gl::Context* context;
  ldk::gl::Shader shader;
  ldk::gl::Renderable renderable;
  ldk::gl::VertexBuffer buffer;
  ldk::gl::DrawCall drawCall;
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
    out_color = mix(solidColor, textureColor, 0.5);
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


  _gameState->context = ldk::gl::createContext(255, GL_COLOR_BUFFER_BIT ,0);
  ldk::gl::makeVertexBuffer(&_gameState->buffer, 64, VERTEX_SIZE);
  ldk::gl::addVertexBufferAttribute(&_gameState->buffer, "_pos", 3, ldk::gl::VertexAttributeType::FLOAT, 0);
  ldk::gl::addVertexBufferAttribute(&_gameState->buffer, "_uuv", 2, ldk::gl::VertexAttributeType::FLOAT,  3 * sizeof(float));
  ldk::gl::loadShader(&_gameState->shader, vs, fs);

  _gameState->bmpTexture = ldk::loadBitmap("Assets/test.bmp");
  if(_gameState->bmpTexture)
  {
    LogInfo( "Loaded bmp %dx%d"
        ,_gameState->bmpTexture->width
        ,_gameState->bmpTexture->height);
  }

  int32 textureId = ldk::gl::createTexture(_gameState->bmpTexture);
  //ldk::freeAsset((void*) _gameState->bmpTexture);
  ldk::gl::setShaderTexture(&_gameState->shader, "_mainTexture", &textureId);


  uint32 maxIndices = (sizeof(indices) / sizeof (uint32));
  ldk::gl::makeRenderable(&_gameState->renderable, &_gameState->buffer, indices, maxIndices, true);
  ldk::gl::setShader(&_gameState->renderable, &_gameState->shader);

  // compose draw call
  _gameState->drawCall.renderable = &_gameState->renderable;
  _gameState->drawCall.type = gl::DrawCall::DRAW_INDEXED;
  _gameState->drawCall.textureCount = 1;
  _gameState->drawCall.textureId[0] = textureId;
  //_gameState->drawCall.vertexCount = 36;
  _gameState->drawCall.vertexCount = 6;
  _gameState->drawCall.vertices = mesh;
  _gameState->drawCall.indexStart = 0;
  _gameState->drawCall.indexCount = maxIndices;
  _gameState->initialized = 1;

  // projection
  _gameState->projMatrix.perspective(RADIAN(45), 4/3, 50.0f, -50.0f);
  ldk::gl::setShaderMatrix4(&_gameState->shader, "mprojection", &_gameState->projMatrix);

  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{10.0, 10.0, 10.0});
  _gameState->modelMatrix.translate(Vec3{0, 0, -10});
  ldk::gl::setShaderMatrix4(&_gameState->shader, "mmodel", &_gameState->modelMatrix);
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
    ldk::gl::setShaderMatrix4(&_gameState->shader, "mmodel", &_gameState->modelMatrix);
  }
#else
    _gameState->modelMatrix.rotate(1,1,1, RADIAN(45.0f) * deltaTime);
    ldk::gl::setShaderMatrix4(&_gameState->shader, "mmodel", &_gameState->modelMatrix);
#endif

  ldk::gl::pushDrawCall(_gameState->context, &_gameState->drawCall);
  ldk::gl::flush(_gameState->context);
}

void gameStop()
{
  ldk::gl::destroyContext(_gameState->context);
}
