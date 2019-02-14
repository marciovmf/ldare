#include <ldk/ldk.h>
#include <stdlib.h>

#define MAX_SPRITES 50000 * 2
using namespace ldk;

struct GameState
{
  uint32 initialized;
  renderer::Sprite sprite;
  renderer::Material material;
  renderer::Texture texture;
  renderer::Context* context;
  renderer::SpriteBatch* spriteBatch;
  Mat4 modelMatrix;
  Mat4 projMatrix;
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

  srand((uint64)memory << 32);

  if (_gameState->initialized)
    return;

  _gameState->context =
  renderer::createContext(255, renderer::Context::COLOR_BUFFER | renderer::Context::DEPTH_BUFFER, 0);

  // Load Textures from bmp
  auto bmp = loadBitmap("Assets/ldk.bmp");
 
  _gameState->texture = renderer::createTexture(bmp,
      renderer::TextureFilter::NEAREST,
      renderer::TextureFilter::NEAREST,
      renderer::TextureWrap::CLAMPTOEDGE);

  freeAsset((void*) bmp);

  // Initialize material
  makeMaterial(&_gameState->material, vs, fs);
  renderer::setTexture(&_gameState->material, "_mainTexture", _gameState->texture); 

  // Calculate matrices and send them to shader uniforms  
  // projection 
  _gameState->projMatrix.orthographic(0, 800, 0, 800, -10, 10);
  renderer::setMatrix4(&_gameState->material, "mprojection", &_gameState->projMatrix);
  // model
  _gameState->modelMatrix.identity();
  _gameState->modelMatrix.scale(Vec3{1.0, 1.0, 1.0});
  _gameState->modelMatrix.translate(Vec3{0, 0, -2});
  renderer::setMatrix4(&_gameState->material, "mmodel", &_gameState->modelMatrix);

  // Initialize the sprite batch
  _gameState->spriteBatch = renderer::createSpriteBatch( _gameState->context, MAX_SPRITES);
  renderer::makeSprite(&_gameState->sprite, &_gameState->material,  1, 1, 127, 127);
}

void gameUpdate(float deltaTime) 
{
  renderer::spriteBatchBegin(_gameState->spriteBatch);
  for(uint32 i=0; i < MAX_SPRITES; i++)
  {
    // Random position, rotation and scale
    float randomX = (rand() % 1920); 
    float randomY = (rand() % 1080);
    float randomScale = (rand() % 100) + 1;
    float randomAngle = RADIAN(rand() % 270);
    renderer::spriteBatchDraw(_gameState->spriteBatch, &_gameState->sprite, 
        randomX - 64, 
        randomY - 64, 
        randomScale, randomScale,
        randomAngle, randomX, randomY);
  }

//  renderer::spriteBatchDraw(_gameState->spriteBatch, &_gameState->sprite,
//      150,
//      150,
//      2.0f, 2.0f);
  renderer::spriteBatchEnd(_gameState->spriteBatch);
}

void gameStop()
{
  ldk::renderer::destroyTexture(_gameState->texture);
  ldk::renderer::destroyContext(_gameState->context);
  ldk::renderer::destroySpriteBatch(_gameState->spriteBatch);
}
