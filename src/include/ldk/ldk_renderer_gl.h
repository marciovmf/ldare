#ifndef _LDK_RENDERER_GL_H_
#define _LDK_RENDERER_GL_H_

#define LDK_GL_MAX_VERTEX_ATTRIBUTES  16
#define LDK_GL_UNIFORM_NAME_LENGTH    64
#define LDK_GL_MAX_UNIFORM_COUNT      8
#define LDK_GL_MAX_TEXTURES           8
#define LDK_GL_NUM_VBOS               3

#include "gl/glcorearb.h"

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
      bool isBound;
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

      Vec4 clearColor;
      uint32 clearBits;
      uint32 settingsBits;
      uint32 maxDrawCalls;
      uint32 drawCallCount;
      DrawCall* drawCalls;
      uint32 loadedTextures;
      Mat4 projectionMatrix;
      bool initialized;
    };

    struct Renderable
    {
      VertexBuffer buffer;
      ldk::HMaterial materialHandle;
      ldk::HMesh meshHandle;       // **If** this renderable was build from a mesh, this is a reference to it.
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
      Mat4 modelMatrix;
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

    ///@brief Returns the rendering context
    ///@returns A pointer to the context
    LDK_API Context* context_get();

    ///@brief Initializes the rendering context
    ///@param maxDrawCalls - Maximum number of draw calls per frame
    ///@param clearBits - What buffers to clear every frame. @see Context
    ///@returns A pointer to the context
    LDK_API void context_initialize(uint32 maxDrawCalls,
        const Vec4& clearColor,
        uint32 settingsBits);

    LDK_API void context_setClearColor(const Vec4& color);

    LDK_API void clearBuffers(uint32 clearBits);

    LDK_API void beginFrame(Mat4& projection);

    LDK_API void setViewPort(Rect& viewport);

    ///@brief Destroys a rendering context
    ///@param Context - The context to destroy
    ///@param name - The name of the section to get
    LDK_API void context_destroy(Context* context);

    ///@brief Initializes a Material with the given shaders
    ///@param material - a pointer to a Material struct. @see Material.
    ///@param vertexSource - vertex shader source;
    ///@param fragmentSource - fragment shader source;
    ///@returns true if shaders compile successfuly
    LDK_API bool  makeMaterial(Material* material, char* vertexSource, char* fragmentSource, uint32 renderQueue = RENDER_QUEUE_OPAQUE);

    ///@brief Assigns a material to a renderable
    ///@param renderable - The Renderable to assing a shader to
    ///@param Material - Handle to the material
    //LDK_API void setShader(Renderable* renderable, Shader* shader);
    LDK_API void renderable_setMaterial(Renderable* renderable, HMaterial materialHandle);

    ///@brief Set a Texture id shader parameter
    ///@param materialHandle - Handle to the material
    ///@param name - The shader parameter to set the value
    ///@param matrix - The matrix parameter value to set
    LDK_API bool material_setTexture(HMaterial materialHandle, char* name, Texture texture);

    ///@brief Set a Matrix4 shader parameter.
    ///@param material - Handle to the material
    ///@param name - The shader parameter to set the value
    ///@param matrix - The matrix parameter value to set
    LDK_API void material_setMatrix4(HMaterial materialHandle, char* name, ldk::Mat4* matrix);

    ///@brief Set an integer shader parameter.
    ///@param materialHandle - Handle to the material
    ///@param name - The integer parameter
    ///@param intParam - The Matrix4 value to set
    LDK_API void material_setInt(HMaterial materialHandle, char* name, uint32 intParam);

    ///@brief Set an integer compound shader parameter.
    ///@param materialHandle - Handle to the material
    ///@param name - The integer parameter
    ///@param count - The number of integer components to set
    ///@param intParam - An array of integer values to set. This array length must be equals to count.
    LDK_API void material_setInt(HMaterial materialHandle, char* name, uint32 count, uint32* intParam);

    ///@brief Set a float shader parameter.
    ///@param Material - Handle to the material
    ///@param name - The float parameter
    ///@param floatParam - The float value to set
    LDK_API void material_setFloat(HMaterial materialHandle, char* name, float floatParam);

    ///@brief Set a float compound shader parameter.
    ///@param Material - Handle to the material
    ///@param name - The float parameter
    ///@param count - The number of float components to set
    ///@param floatParam - An array of float values to set. This array length must be equals to count.
    LDK_API void material_setFloat(HMaterial materialHandle, char* name, uint32 count, float* floatParam);

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

    ///@brief Creates a rendereable from a mesh.
    ///@param meshHandle - Handle to mesh.
    ///@param materialHandle - Handle to material.
    ///@returns a Handle to the renderable.
    LDK_API ldk::HRenderable renderable_create(ldk::HMesh meshHandle, ldk::HMaterial materialHandle);

    ///@brief Set the renderable model matrix
    ///@param renderable - Handle to renderable.
    ///@param modelMatrix - Model matrix for the renderable.
    ///@returns a Handle to the renderable.
    LDK_API void renderable_setMatrix(ldk::HRenderable renderableHandle, const Mat4* modelMatrix);

    ///@brief Creates a material from a mesh.
    ///@param meshHandle - Handle to mesh.
    ///@param materialHandle - Handle to material.
    ///@returns a Handle to the renderable.
    LDK_API ldk::HMaterial loadMaterial(const char* file);

    ///@brief Submits a draw call for execution.
    ///@param context - The rendering context to push the draw call into
    ///@param drawCall - The draw call to push.
    LDK_API void pushDrawCall(DrawCall* drawCall);

    ///@brief Composes and submits a drawcall for a drawing a given renderable
    ///@param context - The rendering context to push the draw call into
    ///@param drawCall - The draw call to push.
    ///@see Handle
    LDK_API void drawIndexed(ldk::HRenderable renderable);

    ///@brief Draws all queued drawcalls forcing draw calls to execute.
    ///@param context - Rendering contex to flush draw calls
    LDK_API void endFrame();

    ///@brief Crates a gpu texture from a bitmap
    ///@param bitmap - The bitmap to create the texture from
    ///@param minFilter - min filter
    ///@param magFilter - max filter
    ///@param uWrap - wrap mode on u axis
    ///@param vWrap - wrap mode on v axis
    ///@returns the gpu texture 
    LDK_API Texture createTexture(ldk::HBitmap handle
        ,TextureFilter minFilter = TextureFilter::LINEAR
        ,TextureFilter magFilter = TextureFilter::LINEAR
        ,TextureWrap uWrap = TextureWrap::CLAMPTOEDGE
        ,TextureWrap vWrap = TextureWrap::CLAMPTOEDGE);

    ///@brief Destroys a gpu texture. Use this to release gpu memory.
    ///@param texture - The gpu texture id.
    LDK_API void destroyTexture(Texture& texture);

    ///@brief Destroys a Material
    ///@param materialHandle - handle to the material to destroy
    LDK_API void material_destroy(ldk::HMaterial materialHandle);

    ///@brief Destroys a Renderable
    ///@param renderableHandle - handle to the renderable to destroy
    LDK_API void renderable_destroy(ldk::HRenderable renderableHandle);

    ///@brief Finalizes the rendering context
    LDK_API void context_finalize();

///@}
  } // renderer
} // ldk

#endif// _LDK_RENDERER_GL_H_

