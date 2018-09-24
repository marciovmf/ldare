#include <ldk/ldk.h>

#define STR(s) #s

using namespace ldk;

float mesh[] = {
  -0.5f,  -0.5f,  0.0f,
  0.5f,   -0.5f,  0.0f,
  0.0f,   0.5f,   0.0f
};

ldk::gl::Context* context;
ldk::gl::Shader shader;
ldk::gl::Renderable renderable;
ldk::gl::VertexBuffer buffer;
ldk::gl::DrawCall drawCall;

// Vertex shader
char* vs = STR(#version 330\n
  layout(location = 0) in vec3 _pos;\n
  void main()\n
  {
      gl_Position = vec4(_pos, 1.0);\n
  });

// Fragment shader
char* fs = STR(#version 330\n
  out vec4 out_color;\n
  void main()\n
  {
      out_color = vec4(1.0, 0.3, 0.0, 1.0);\n
  });


void gameInit(void* memory) { }

void gameStart()
{
  context = ldk::gl::createContext(255, GL_COLOR_BUFFER_BIT,0);
  
  ldk::gl::makeVertexBuffer(&buffer, 3 * sizeof(float), 3 * sizeof(float));
  ldk::gl::addVertexBufferAttribute(&buffer, "_pos", 3, GL_FLOAT, 0);

  ldk::gl::loadShader(&shader, vs, fs);
  ldk::gl::makeRenderable(&renderable, &buffer, true);

  ldk::gl::setShader(&renderable, &shader);

  // compose draw call
  drawCall.renderable = &renderable;
  drawCall.textureCount = 0;
  drawCall.vertexCount = 3;
  drawCall.vertices = mesh;

}

void gameUpdate(float deltaTime) 
{
  ldk::gl::pushDrawCall(context, &drawCall);
  ldk::gl::flush(context);
}

void gameStop()
{
  ldk::gl::destroyContext(context);
}
