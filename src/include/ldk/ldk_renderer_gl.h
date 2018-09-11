#ifndef _LDK_RENDERER_GL_H_
#define _LDK_RENDERER_GL_H_

#define LDK_RENDER_NUM_BUFFERS    3
#define LDK_MAX_UNIFORM_COUNT     8
#define LDK_MAX_ATTRIBUTES        16
#define LDK_UNIFORM_NAME_LENGTH   64

namespace ldk
{
  struct Uniform
  {
    char name[LDK_UNIFORM_NAME_LENGTH];
    uint32 id;
    uint32 hash;
    uint32 type;
    uint32 location;
    uint32 size;
  };

  struct Shader
  {
    uint32 program;
    uint32 uniformCount;
    Uniform uniforms[LDK_MAX_UNIFORM_COUNT];
  };

  struct DrawCall
  {
    uint32 vertexCount;
    void* vertices;
    uint32 textureCount;
    uint32 textureId[8];
  };

  struct RenderContext
  {
    uint32 clearBits;
    uint32 settingsBits;
    uint32 maxDrawCalls;
    uint32 drawCallCount;
    DrawCall* drawCalls;
  };

  struct VertexAttribute
  {
    const char* name;
    uint64 hash;
    uint32 size;
    uint32 type;
    uint32 offset;
    uint32 location;
  };

  struct VertexBufferLayout
  {
    uint32 size;
    uint32 stride;
    uint32 usage;
    uint32 attributCount;
    VertexAttribute attributes[LDK_MAX_ATTRIBUTES];
  };

  struct Renderable
  {
    VertexBufferLayout data;
    Shader* shader;
    //TODO: add rendere queue order info herer
    uint32 attributeCount;
    uint32 index0;
    uint32 index1;
    uint32 needUpdate;
    uint32 bufferNumber;
    uint32 bufferCount;
    uint32 buffers[LDK_RENDER_NUM_BUFFERS];
    //GLsync fences[LDK_RENDER_NUM_BUFFERS];
    size_t fences[LDK_RENDER_NUM_BUFFERS];
  };
  
  enum VertexAttributeType
  {
    ATTRIB_TYPE_FLOAT,
    ATTRIB_TYPE_INT,
    ATTRIB_TYPE_BOOL,
    ATTRIB_TYPE_SAMPLER,
    ATTRIB_TYPE_UNKNOWN
  };
  
  LDK_API int32 loadShader(Shader* shader, char* vertexSource, char* fragmentSource);

  LDK_API RenderContext* makeRenderContext(uint32 maxDrawCalls, uint32 clearBits, uint32 settingsBits);

  LDK_API void freeContext(RenderContext* context);

  LDK_API void setVertexBufferLayout(VertexBufferLayout* data, uint32 bufferSize, uint32 stride, bool isStatic);

  LDK_API void makeRenderable(Renderable* renderable, VertexBufferLayout* vertexData);

  LDK_API void setShader(Renderable* renderable, Shader* shader);

} // ldk

#endif// _LDK_RENDERER_GL_H_

