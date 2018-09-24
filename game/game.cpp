#include <ldk/ldk.h>

#define STR(s) #s

using namespace ldk;

float mesh[] = {
  -0.5f,	-0.5f,	0.0f, 1.0f, 0.0f, 0.0f,
  0.5f,		-0.5f,	0.0f, 0.0f, 1.0f, 0.0f,
  0.0f,		0.5f,	0.0f, 0.0f, 0.0f, 1.0f,
};

#define VERTEX_SIZE (6 * sizeof(float))

ldk::gl::Context* context;
ldk::gl::Shader shader;
ldk::gl::Renderable renderable;
ldk::gl::VertexBuffer buffer;
ldk::gl::DrawCall drawCall;

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
	  //out_color = vec4(1.0, fragColor.x, 0.0, 1.0) ; \n
	out_color = vec4(fragColor, 1.0); \n
  });

void gameInit(void* memory) { }

void gameStart()
{
  context = ldk::gl::createContext(255, GL_COLOR_BUFFER_BIT,0);
  
  ldk::gl::makeVertexBuffer(&buffer, 3, VERTEX_SIZE);
  ldk::gl::addVertexBufferAttribute(&buffer, "_pos", 3, GL_FLOAT, 0);
  ldk::gl::addVertexBufferAttribute(&buffer, "_color", 3, GL_FLOAT,  3 * sizeof(float));

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
