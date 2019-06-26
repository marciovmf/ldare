#ifndef _LDK_RENDERER_GL_H_
#define _LDK_RENDERER_GL_H_

#define LDK_GL_MAX_VERTEX_ATTRIBUTES  16
#define LDK_GL_UNIFORM_NAME_LENGTH    64
#define LDK_GL_MAX_UNIFORM_COUNT      8
#define LDK_GL_MAX_TEXTURES           8
#define LDK_GL_NUM_VBOS               3

#include "../GL/glcorearb.h"

namespace ldk
{
  namespace renderer
  {
/// @defgroup Renderer Renderer
/// @{

    struct Shader;
    struct DrawCall;
    struct Context;
    struct VertexBuffer;

    const uint32 RENDER_QUEUE_OPAQUE = 10;
    const uint32 RENDER_QUEUE_TRANSLUCENT = 100;
    const uint32 RENDER_QUEUE_OVERLAY = 200;

    enum VertexAttributeType
    {
      FLOAT = 1,
      BYTE, 
      UNSIGNED_BYTE,
      SHORT,
      UNSIGNED_SHORT,
      INT,
      BOOL,
      UNSIGNED_INT,
      DOUBLE,
      SAMPLER,
      UNKNOWN = -1
    };

    struct VertexAttribute
    {
      const char* name;
      uint64 hash;
      GLuint size;
      VertexAttributeType type;
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

    enum TextureFilter
    {
      LINEAR,
      NEAREST,
      MIPMAPLINEARLINEAR,
      MIPMAPLINEARNEAREST,
      MIPMAPNEARESTLINEAR,
      MIPMAPNEARESTNEAREST,
    };

    enum TextureWrap
    {
      CLAMPTOEDGE,
      MIRROREDREPEAT,
      REPEAT 
//      CLAMPTOBORDER ?
    };

    struct Texture
    {
      uint32 id;
      uint16 width;
      uint16 height;
      TextureFilter minFilter;
      TextureFilter magFilter;
      TextureWrap uWrap;
      TextureWrap vWrap;
    };

    struct Material
    {
      Shader shader; 
      uint32 renderQueue; 
      GLuint textureCount;
      Texture texture[LDK_GL_MAX_TEXTURES];
    };

    struct VertexBuffer
    {
      GLuint capacity;
      GLuint stride;
      GLuint attributeCount;
      GLuint primitive;
      VertexAttribute attributes[LDK_GL_MAX_VERTEX_ATTRIBUTES];
    };

    struct Context
    {
      static const uint32 COLOR_BUFFER = GL_COLOR_BUFFER_BIT;
      static const uint32 DEPTH_BUFFER = GL_DEPTH_BUFFER_BIT;
      static const uint32 STENCIL_BUFFER = GL_STENCIL_BUFFER_BIT;

      uint32 clearBits;
      uint32 settingsBits;
      uint32 maxDrawCalls;
      uint32 drawCallCount;
      DrawCall* drawCalls;
      uint32 loadedTextures;
    };

    struct Renderable
    {
      VertexBuffer buffer;
      ldk::Handle materialHandle;
      ldk::Handle meshHandle;       // **If** this renderable was build from a mesh, this is a reference to it.
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
      void* vertices;
      Renderable* renderable;
      GLuint indexStart;
      GLuint vertexCount;
      GLuint indexCount;
      enum 
      {
        DRAW = 0,
        DRAW_INDEXED
        // 
      } type;
    };

    ///@brief Creates arendering context
    ///@param maxDrawCalls - Maximum number of draw calls per frame
    ///@param clearBits - What buffers to clear every frame. @see Context
    ///@returns A pointer to the context
    LDK_API Context* createContext(uint32 maxDrawCalls, uint32 clearBits, uint32 settingsBits);

    ///@brief Destroys a rendering context
    ///@param Context - The context to destroy
    ///@param name - The name of the section to get
    LDK_API void destroyContext(Context* context);

    ///@brief Initializes a Material with the given shaders
    ///@param material - a pointer to a Material struct. @see Material.
    ///@param vertexSource - vertex shader source;
    ///@param fragmentSource - fragment shader source;
    ///@returns true if shaders compile successfuly
    LDK_API bool  makeMaterial(Material* material, char* vertexSource, char* fragmentSource, uint32 renderQueue = RENDER_QUEUE_OPAQUE);

    LDK_API bool setTexture(Handle materialHandle, char* name, Texture texture);

    ///@brief Assigns a material to a renderable
    ///@param renderable - The Renderable to assing a shader to
    ///@param Material - Handle to the material
    //LDK_API void setShader(Renderable* renderable, Shader* shader);
    LDK_API void setMaterial(Renderable* renderable, Handle materialHandle);

    ///@brief Set a Matrix4 shader parameter.
    ///@param Material - Handle to the material
    ///@param name - The shader parameter to set the value
    ///@param matrix - The matrix parameter value to set
    LDK_API void setMatrix4(Handle material, char* name, ldk::Mat4* matrix);

    ///@brief Set an integer shader parameter.
    ///@param Material - Handle to the material
    ///@param name - The integer parameter
    ///@param intParam - The Matrix4 value to set
    LDK_API void setInt(Handle material, char* name, uint32 intParam);

    ///@brief Set an integer compound shader parameter.
    ///@param Material - Handle to the material
    ///@param name - The integer parameter
    ///@param count - The number of integer components to set
    ///@param intParam - An array of integer values to set. This array length must be equals to count.
    LDK_API void setInt(Handle materialHandle, char* name, uint32 count, uint32* intParam);

    ///@brief Set a float shader parameter.
    ///@param Material - Handle to the material
    ///@param name - The float parameter
    ///@param floatParam - The float value to set
    LDK_API void setFloat(Handle materialHandle, char* name, float floatParam);

    ///@brief Set a float compound shader parameter.
    ///@param Material - Handle to the material
    ///@param name - The float parameter
    ///@param count - The number of float components to set
    ///@param floatParam - An array of float values to set. This array length must be equals to count.
    LDK_API void setFloat(Handle materialHandle, char* name, uint32 count, float* floatParam);

    ///@brief Initializes a VertexBuffer structure.
    ///@param buffer - The vertex buffer structur to initialize
    ///@param capacity - maximum number of vertex entries on this buffer. The size of a single vertex depends on the buffer attributes.
    LDK_API void makeVertexBuffer(VertexBuffer* buffer, uint32 capacity);

    ///@brief Adds an attribute to a VertexBuffer. @see VertexBuffer
    ///@param buffer - The buffer to add the attribute to
    ///@param name - Name of the attribute
    ///@param size - The number of components per generic vertex attribute. Must be 1,2,3 or 4
    ///@param type - Vertex attribute data type. @see VertexAttributeType
    ///@param offset - Offset of the first occurrence of this attribute on the vertex buffer.
    LDK_API void addVertexBufferAttribute(VertexBuffer* buffer, char* name, uint32 size, VertexAttributeType type, uint32 offset);

    ///@brief Initializes a rendereable structure.
    ///@param renderable - The renderable to initialize
    ///@param vertexBuffer - A vertex buffer associated to this renderable.
    ///@param isStatic - True if this is a static renderable, false otherwise. A
    //static renderable is uploaded to the GPU only once instead of every frame
    LDK_API void makeRenderable(Renderable* renderable, VertexBuffer* vertexBuffer, bool isStatic);

    ///@brief Initializes a rendereable structure.
    ///@param renderable - The renderable to initialize
    ///@param vertexBuffer - A vertex buffer associated to this renderable
    ///@param indices - An array of indices for indexing the vertex buffer
    ///@param maxIndexCount - Maximum size of the index buffer
    ///@param isStatic - True if this is a static renderable, false otherwise. A
    //static renderable is uploaded to the GPU only once instead of every frame
    LDK_API void makeRenderable(Renderable* renderable, VertexBuffer* vertexBuffer, uint32* indices, uint32 maxIndexCount, bool isStatic);

    LDK_API ldk::Handle makeRenderable(ldk::Handle meshHandle, ldk::Handle materialHandle);

    ///@brief Submits a draw call for execution.
    ///@param context - The rendering context to push the draw call into
    ///@param drawCall - The draw call to push.
    LDK_API void pushDrawCall(Context* context, DrawCall* drawCall);

    LDK_API void drawIndexed(Context* context, ldk::Handle renderable);

    ///@brief Flushes the draw call queue forcing draw calls to execute.
    ///@param context - Rendering contex to flush draw calls
    LDK_API void flush(Context* context);

    ///@brief Crates a gpu texture from a bitmap
    ///@param bitmap - The bitmap to create the texture from
    ///@param minFilter - min filter
    ///@param magFilter - max filter
    ///@param uWrap - wrap mode on u axis
    ///@param vWrap - wrap mode on v axis
    ///@returns the gpu texture 
    LDK_API Texture createTexture(ldk::Handle handle
        ,TextureFilter minFilter = TextureFilter::LINEAR
        ,TextureFilter magFilter = TextureFilter::LINEAR
        ,TextureWrap uWrap = TextureWrap::CLAMPTOEDGE
        ,TextureWrap vWrap = TextureWrap::CLAMPTOEDGE);

    ///@brief Destroys a gpu texture. Use this to release gpu memory.
    ///@param texture - The gpu texture id.
    LDK_API void destroyTexture(Texture& texture);

    LDK_API ldk::Handle loadMaterial(const char* file);
    
    LDK_API void destroyMaterial(ldk::Handle materialHandle);
    
///@}
  } // renderer
} // ldk

#endif// _LDK_RENDERER_GL_H_

