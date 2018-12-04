#ifndef _LDK_RENDERER_GL_H_
#define _LDK_RENDERER_GL_H_

#define LDK_GL_MAX_VERTEX_ATTRIBUTES  16
#define LDK_GL_UNIFORM_NAME_LENGTH    64
#define LDK_GL_MAX_UNIFORM_COUNT      8
#define LDK_GL_MAX_TEXTURES           8
#define LDK_GL_NUM_VBOS               3


namespace ldk
{
  enum RenderQueue
  {
    OPAQUE = 10,
    TRANSLUCENT = 100,
    OVERLAY = 200,
  };
}

#include "../GL/glcorearb.h"

namespace ldk
{

  namespace gl
  {
    struct Shader;
    struct DrawCall;
    struct Context;
    struct VertexBuffer;

    enum VertexAttributeType
    {
      FLOAT = 1,
      INT,
      BOOL,
      SAMPLER,
      UNKNOWN 
    };

    struct VertexAttribute
    {
      const char* name;
      uint64 hash;
      GLuint size;
      GLuint type;
      GLint offset;
      GLuint location;
    };

    struct Uniform
    {
      char name[LDK_GL_UNIFORM_NAME_LENGTH];
      GLuint id;
      GLuint hash;
      GLuint type;
      GLint location;
      GLuint size;
    };

    struct Shader
    {
      GLuint program;
      GLuint uniformCount;
      Uniform uniforms[LDK_GL_MAX_UNIFORM_COUNT];
    };

    struct VertexBuffer
    {
      GLuint size;
      GLuint stride;
      GLuint attributeCount;
      GLuint primitive;
      VertexAttribute attributes[LDK_GL_MAX_VERTEX_ATTRIBUTES];
    };

    struct Context
    {
      uint32 clearBits;
      uint32 settingsBits;
      uint32 maxDrawCalls;
      uint32 drawCallCount;
      DrawCall* drawCalls;
    };

    struct Renderable
    {
      VertexBuffer buffer;
      Shader* shader;
      RenderQueue renderQueue; 
      GLuint ibo;
      GLuint iboSize;
      GLuint attributeCount;
      GLuint index0;
      GLuint index1;
      GLuint currentVboIndex;
      GLuint needNewSync;
      GLuint vboCount;
      GLuint vbos[LDK_GL_NUM_VBOS];
      GLsync fences[LDK_GL_NUM_VBOS];
      uint32 usage;
    };

    struct DrawCall
    {
      enum 
      {
        DRAW = 0,
        DRAW_INDEXED
        // 
      } type;

      void* vertices;
      Renderable* renderable;
      GLuint indexStart;
      GLuint vertexCount;
      GLuint indexCount;
      GLuint textureCount;
      GLuint textureId[LDK_GL_MAX_TEXTURES];
    };

    LDK_API Context* createContext(uint32 maxDrawCalls, uint32 clearBits, uint32 settingsBits);
    LDK_API void destroyContext(Context* context);
    LDK_API bool loadShader(Shader* shader, char* vertexSource, char* fragmentSource);
    LDK_API void setShader(Renderable* renderable, Shader* shader);
    LDK_API void setShaderMatrix4(Shader* shader, char* name, ldk::Mat4* matrix);
    LDK_API void setShaderInt(Shader* shader, char* name, uint32 count, uint32* intParam);
    LDK_API void setShaderFloat(Shader* shader, char* name, uint32 count, float* floatParam);
    LDK_API void makeVertexBuffer(VertexBuffer* buffer, uint32 bufferSize, uint32 stride);
    LDK_API void addVertexBufferAttribute(VertexBuffer* buffer, char* name, uint32 size, VertexAttributeType type, uint32 offset);
    LDK_API void makeRenderable(Renderable* renderable, VertexBuffer* vertexBuffer, bool isStatic);
    LDK_API void makeRenderable(Renderable* renderable, VertexBuffer* vertexBuffer, uint32* indices, uint32 maxIndexCount, bool isStatic);
    LDK_API void pushDrawCall(Context* context, DrawCall* drawCall);
    LDK_API void flush(Context* context);
  } // gl
} // ldk

#endif// _LDK_RENDERER_GL_H_

