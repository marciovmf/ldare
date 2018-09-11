#include <ldk/ldk.h>

#define STR(s) #s

static ldk::RenderContext* context;
ldk::Renderable;
ldk::Shader shader;
ldk::VertexData vertexData;
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
  context =  ldk::makeRenderContext(100, 0, 0);
  ldk::loadShader(&shader, vs, fs);
  ldk::setVertexData(&vertexData, 6, 3 * sizeof(float), true);
}

void gameUpdate(float deltaTime) { }

void gameStop()
{
  ldk::freeContext(context);
}
