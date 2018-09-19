#define LDK_RENDERER_NUM_BUFFERS            3
#define LDK_RENDERER_MAX_TEXTURES           8

#define LDK_RENDERER_ALLOC(size) malloc((size))
#define LDK_RENDERER_FREE(ptr) free((ptr))

namespace ldk
{
  namespace renderer
  {
    struct Material
    {
      Shader shader;
      uint32 textureCount;
      uint32 textureId[LDK_RENDERER_MAX_TEXTURES];
      int32 renderQueue; 
      RenderBuffer* buffer;
      //TODO: Marcio, remember that the drawcall references the buffer so
      // we can minimize state changes per draw call.
      // DrawCalls will be ordered, first by renderQueue, than by buffer
    };

    struct DrawCall
    {
      uint32 vertexCount;
      void* vertices;
      uint32 textureCount;
      uint32 textureId[LDK_RENDERER_MAX_TEXTURES];
    };

    struct Context
    {
      uint32 clearBits;
      uint32 settingsBits;
      uint32 maxDrawCalls;
      uint32 drawCallCount;
      DrawCall* drawCalls;
    };


    struct RenderBuffer
    {
      RenderBufferLayout layout;
      uint32 usage;
      Shader* shader;
      uint32 attributeCount;
      uint32 index0;
      uint32 index1;
      uint32 needUpdate;
      uint32 bufferNumber;
      uint32 bufferCount;
      uint32 buffers[LDK_RENDERER_NUM_BUFFERS];
      size_t fences[LDK_RENDERER_NUM_BUFFERS];
    };


    //
    // Internal functions
    //

    static GLboolean _checkShaderProgramLink(GLuint program)
    {
      GLint success = 0;
      GLchar msgBuffer[1024] = {};
      glGetProgramiv(program, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(program, sizeof(msgBuffer), 0, msgBuffer);
        LogError("Error linking shader");
        LogError(msgBuffer);
        return GL_FALSE;
      }

      return GL_TRUE;
    }

    static GLboolean _checkShaderCompilation(GLuint shader)
    {
      GLint success = 0;
      GLchar msgBuffer[512] = {};

      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        GLint shaderType;
        const char* shaderTypeName;

        glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
        glGetShaderInfoLog(shader, sizeof(msgBuffer), 0, msgBuffer);

        if ( shaderType == GL_VERTEX_SHADER)
          shaderTypeName = "Error compiling vertex shader";
        else if ( shaderType == GL_FRAGMENT_SHADER)
          shaderTypeName = "Error compiling fragment shader";
        else
          shaderTypeName = "Error compiling unknown shader type";

        LogError(shaderTypeName);
        LogError(msgBuffer);
        return GL_FALSE;
      }

      return GL_TRUE;
    }

    static GLuint _compileShader(const char* source, GLenum shaderType)
    {
      LDK_ASSERT(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER,
          "Invalid shader type");

      // Setup default vertex shader
      uint32 shader = glCreateShader(shaderType);
      glShaderSource(shader, 1, (const GLchar**)&source, 0);
      glCompileShader(shader);
      if (!_checkShaderCompilation(shader))
        return GL_FALSE;

      return shader;
    }

    static GLuint _createShaderProgram(const char* vertex, const char* fragment)
    {
      uint32 vertexShader = _compileShader((const char*)vertex, GL_VERTEX_SHADER);
      uint32 fragmentShader = _compileShader((const char*)fragment, GL_FRAGMENT_SHADER);
      uint32 shaderProgram = glCreateProgram();

      glAttachShader(shaderProgram, vertexShader);	
      glAttachShader(shaderProgram, fragmentShader);

      // Link shader program
      glLinkProgram(shaderProgram);
      if (!_checkShaderProgramLink(shaderProgram))
        return GL_FALSE;

      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      return shaderProgram;
    }

    RenderBufferAttributeType _glTypeToInternal(uint32 glType)
    {
      switch (glType)
      {
        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
          return ldk::renderer::RenderBufferAttributeType::INT;

        case GL_FLOAT:
        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT4:
          return RenderBufferAttributeType::FLOAT;

        case GL_BOOL:
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
          return RenderBufferAttributeType::BOOL;

        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
          return RenderBufferAttributeType::SAMPLER;

        default:
          return RenderBufferAttributeType::UNKNOWN;
      }

    }


    //
    // Material functions
    //

    Material* createMaterial(Shader* shader)
    {
      Material* material = (Material*) LDK_RENDERER_ALLOC(sizeof(Material));
      material->shader = *shader;
      return material;
    }

    void freeMaterial(Material* material)
    {
      LDK_RENDERER_FREE(material);
    }

    //TODO:We need a way to add textures to a material. This is how I thing i
    //should be:
    // Texture loadTexture(Context* context, ImageAsset* imageAsset);
    // - setTexture(Material* materia, Texture);
    // - The context holds all loaded texture ids and Image references, so it can
    // unload on demand and delete the textures if/when necessary

    //
    // Shader functions
    //

    bool loadShader(Shader* shader, char* vertexSource, char* fragmentSource)
    {
      LogInfo(vertexSource);
      LogInfo(fragmentSource);
      uint32 vertexShader = _compileShader((const char*)vertexSource, GL_VERTEX_SHADER);
      uint32 fragmentShader = _compileShader((const char*)fragmentSource, GL_FRAGMENT_SHADER);

      GLuint program = glCreateProgram();
      glAttachShader(program, vertexShader);	
      glAttachShader(program, fragmentShader);

      // Link shader program
      glLinkProgram(program);
      if (!_checkShaderProgramLink(program))
        return GL_FALSE;

      int32 uniformCount;
      glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);

      LDK_ASSERT(uniformCount <= LDK_RENDERER_MAX_UNIFORM_COUNT, "Too many uniforms");

      uint32 maxUniformNameLength = sizeof(char) * LDK_RENDERER_UNIFORM_NAME_LENGTH;

      for (int i = 0; i < uniformCount; i++) 
      {
        uint32 nameLength;
        Uniform uniform = {};

        glGetActiveUniform(program, (GLint)i, maxUniformNameLength, (GLsizei*) &nameLength, (GLsizei*)&uniform.size,
            (GLenum*)&uniform.type, (GLchar*)&uniform.name);

        uniform.location = glGetUniformLocation(program, uniform.name);
        uniform.id = i;
        uniform.hash = stringToHash(uniform.name);
        uniform.type = uniform.type; //TODO: Store an internal representation
        shader->uniforms[i] = uniform;
      }

      glDetachShader(program,vertexShader);
      glDetachShader(program,fragmentShader);
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      shader->program = program;
      shader->uniformCount = uniformCount;
      return program > 0;
    }
    
    void setShader(RenderBuffer* renderable, Shader* shader)
    {
      renderable->shader = shader;
      glGetProgramiv(shader->program, GL_ACTIVE_ATTRIBUTES, (GLint*) renderable->attributeCount);

      if(renderable->attributeCount != renderable->layout.attributeCount)
      {
        LogWarning("Shader and buffer layout has different attribute count");
      }

      uint32 attribType;
      uint32 attribSize = 0;
      uint64 attribHash = 0;
      char attribName[256];

      int32 attribCount = renderable->layout.attributeCount;
      for (int i = 0; i < attribCount; i++) 
      {
        // query attribute type and size
        glGetActiveAttrib(shader->program, i, 256, 0, (GLint*) attribSize,  (GLenum*) &attribType, attribName);
        attribType = _glTypeToInternal(attribType);
        RenderBufferAttribute* attribute;

        for (int j = 0; j < attribCount; j++) 
        {
          RenderBufferAttribute* tmpAttribute = renderable->layout.attributes + j;
          if(tmpAttribute->hash = attribHash)
          {
            attribute = tmpAttribute;
            break;
          }

          LDK_ASSERT(attribute, "No matching attribute found");
          LDK_ASSERT(attribute->type == attribType, "No matching attribute type");

          // cache attribute location
          attribute->location = glGetAttribLocation(shader->program, attribName);
        }
      }

    }

    //
    // Context functions
    //

    Context* makeContext(uint32 maxDrawCalls, uint32 clearBits, uint32 settingsBits)
    {
      Context* context = (Context*) platform::memoryAlloc(sizeof(Context));
      if (!context) return nullptr;

      context->clearBits = clearBits;
      context->settingsBits = settingsBits;
      context->maxDrawCalls = maxDrawCalls;
      context->drawCallCount = 0;
      context->drawCalls = (ldk::renderer::DrawCall*) platform::memoryAlloc(sizeof(ldk::renderer::DrawCall) * maxDrawCalls);

      if(!context->drawCalls)
      {
        platform::memoryFree(context);
        return nullptr;
      }

      GLuint vao;
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      return context;
    }

    void freeContext(Context* context)
    {
      platform::memoryFree(context->drawCalls); 
      platform::memoryFree(context); 
    }

    //
    // Buffer functions
    //

    void makeBufferLayout(RenderBufferLayout* layout, uint32 bufferSize, uint32 stride) 
    {
      layout->size = bufferSize;
      layout->stride = stride;
      layout->attributeCount = 0;
    }

    void addBufferLayoutAttribute(RenderBufferLayout* layout, RenderBufferAttribute* attribute)
    {
      LDK_ASSERT((layout->attributeCount < LDK_RENDERER_MAX_BUFFER_ATTRIBUTES),
          "Maximum attribute count reached for buffer layout");

      int32 index = layout->attributeCount++;
      RenderBufferAttribute* attributeSlot = &layout->attributes[index];
      *attributeSlot = *attribute;
    }

    RenderBuffer* createRenderBuffer(RenderBufferLayout* layout, Primitive primitive, BufferUsage usage)
    {

      RenderBuffer* buffer = (RenderBuffer*) LDK_RENDERER_ALLOC(sizeof(RenderBuffer));
      buffer->layout = *layout;

      if (usage == STATIC_BUFFER)
      {
        buffer->usage = GL_STATIC_DRAW;
        buffer->bufferCount = 1;
      }
      else
      {
        buffer->usage = GL_DYNAMIC_DRAW;
        buffer->bufferCount = LDK_RENDERER_NUM_BUFFERS;
      }

      // Generate GPU buffers
      for (int i = 0; i < buffer->bufferCount; i++) 
      {
        GLuint* vbo = buffer->buffers + i;

        glGenBuffers(1, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        glBufferData(GL_ARRAY_BUFFER, layout->size * layout->stride, NULL, usage);
        buffer->fences[i] = (size_t) glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      return buffer;
    }

    //
    // Render functions
    //
    void pushDrawCall(Context* context, DrawCall* drawCall)
    {
      LDK_ASSERT(context->drawCallCount < context->maxDrawCalls,
          "Exceeded maximum draw calls per frame at current context");
      context->drawCalls[context->drawCallCount++] = *drawCall;
    }


  } // renderer
} // ldk
