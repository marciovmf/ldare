#include <ldk/ldk.h>

#define STR(s) #s

using namespace ldk;

static ldk::renderer::Context* context;
renderer::RenderBuffer;
renderer::Shader shader;
renderer::RenderBufferLayout layout;
float mesh[] = {-0.5,-0.5,0.5,-0.5,0.0,0.5};

// Vertex shader
char* vs = STR(#version 330\n
  layout(location = 0) in vec4 in_position;\n
  void main()\n
  {
      gl_Position = in_position;\n
  });

// Fragment shader
char* fs = STR(#version 330\n
  layout(location = 0) out vec4 out_color;\n
  void main()\n
  {
      out_color = vec4(1.0, 0.0, 0.0, 1.0);\n
  });


void gameInit(void* memory) { }

void gameStart()
{
  context =  ldk::renderer::makeContext(100, 0, 0);
  renderer::loadShader(&shader, vs, fs);
  renderer::makeBufferLayout(&layout, 6, 3 * sizeof(float));
}

void gameUpdate(float deltaTime) { }

void gameStop()
{
  renderer::freeContext(context);
}
