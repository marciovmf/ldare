#ifndef _LDK_RENDERER_GL_H_
#define _LDK_RENDERER_GL_H_

#define LDK_RENDERER_MAX_BUFFER_ATTRIBUTES  16
#define LDK_RENDERER_UNIFORM_NAME_LENGTH    64
#define LDK_RENDERER_MAX_UNIFORM_COUNT      8

namespace ldk
{
  namespace renderer
  {
    struct Shader;
    struct DrawCall;
    struct Context;
    struct RenderBufferAttribute;
    struct RenderBufferLayout;
    struct RenderBuffer;

    struct Uniform
    {
      char name[LDK_RENDERER_UNIFORM_NAME_LENGTH];
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
      Uniform uniforms[LDK_RENDERER_MAX_UNIFORM_COUNT];
    };

    enum RenderQueue
    {
      OPAQUE = 5,
      TRANSLUCENT = 100,
      OVERLAY = 250,
    };

    enum RenderBufferAttributeType
    {
      FLOAT,
      INT,
      BOOL,
      SAMPLER,
      UNKNOWN
    };

    enum Primitive
    {
      PRIMITIVE_TRIANGLES,
      PRIMITIVE_TRIANGLE_STRIP
    };

    enum BufferUsage
    {
      STATIC_BUFFER,
      DYNAMIC_BUFFER
    };

    struct RenderBufferAttribute
    {
      const char* name;
      uint64 hash;
      uint32 size;
      uint32 type;
      uint32 offset;
      uint32 location;
    };

    struct RenderBufferLayout
    {
      uint32 size;
      uint32 stride;
      uint32 attributeCount;
      Primitive primitive;
      RenderBufferAttribute attributes[LDK_RENDERER_MAX_BUFFER_ATTRIBUTES];
    };

    LDK_API bool loadShader(Shader* shader, char* vertexSource, char* fragmentSource);
    LDK_API void setShader(RenderBuffer* renderable, Shader* shader);
    LDK_API Context* makeContext(uint32 maxDrawCalls, uint32 clearBits, uint32 settingsBits);
    LDK_API void freeContext(Context* context);
    LDK_API void makeBufferLayout(RenderBufferLayout* layout, uint32 bufferSize, uint32 stride);
    LDK_API void addBufferLayoutAttribute(RenderBufferLayout* layout, RenderBufferAttribute* attribute);
    LDK_API RenderBuffer* createRenderBuffer(RenderBufferLayout* layout, Primitive primitive, BufferUsage usage);
    LDK_API void pushDrawCall(Context* context, DrawCall* drawCall);

  } // renderer
} // ldk

#endif// _LDK_RENDERER_GL_H_

