
#ifdef _LDK_DEBUG_
#define LDK_ASSERT_VALID_CONTEXT(context) LDK_ASSERT((context)->initialized, "Context is not initialized");
#else
#define LDK_ASSERT_VALID_CONTEXT(context)
#endif

namespace ldk
{
  namespace renderer
  {
    struct Context
    {
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

    static Context _context;

    //valid texture wrap options strings 
    static constexpr char* STR_REPEAT = "repeat";
    static constexpr char* STR_CLAMP_TO_EDGE =  "clamp-to-edge";
    static constexpr char* STR_MIRRORED_REPEAT = "mirrored-repeat";

    //valid texture filter options strings
    static constexpr char* STR_LINEAR = "linear";
    static constexpr char* STR_NEAREST = "nearest";
    static constexpr char* STR_MIP_LINEAR_LINEAR = "mipmap-linear-linear";
    static constexpr char* STR_MIP_LINEAR_NEAREST =  "mipmap-linear-nearest";
    static constexpr char* STR_MIP_NEAREST_LINEAR = "mipmap-nearest-linear";
    static constexpr char* STR_MIP_NEAREST_NEAREST = "mipmap-nearest-nearest";

    // render queue parameters
    static constexpr char* STR_OPAQUE = "opaque";
    static constexpr char* STR_TRANSLUCENT = "translucent";
    static constexpr char* STR_OVERLAY = "overlay";

    // texture parameter keys
    static constexpr char* STR_PARAM_QUEUE = "queue";
    static constexpr char* STR_PARAM_QUEUE_OFFSET = "queue-offset";
    static constexpr char* STR_PARAM_PATH = "path";
    static constexpr char* STR_PARAM_U_WRAP = "u-wrap";
    static constexpr char* STR_PARAM_V_WRAP = "v-wrap";
    static constexpr char* STR_PARAM_MIN_FILTER = "min-filter";
    static constexpr char* STR_PARAM_MAG_FILTER = "mag-filter";

    // texture section names
    static constexpr uint32 texture0 = stringToHash("texture0");
    static constexpr uint32 texture1 = stringToHash("texture1");
    static constexpr uint32 texture2 = stringToHash("texture2");
    static constexpr uint32 texture3 = stringToHash("texture3");
    static constexpr uint32 texture4 = stringToHash("texture4");
    static constexpr uint32 texture5 = stringToHash("texture5");
    static constexpr uint32 texture6 = stringToHash("texture6");
    static constexpr uint32 texture7 = stringToHash("texture7");

    //valid texture wrap options hashes
    static constexpr uint32 wrapRepeat = stringToHash(STR_REPEAT);
    static constexpr uint32 wrapClampToEdge = stringToHash(STR_CLAMP_TO_EDGE);
    static constexpr uint32 wrapMirroredRepeat = stringToHash(STR_MIRRORED_REPEAT);

    //valid texture filter options hashes
    static constexpr uint32 filterLinear = stringToHash(STR_LINEAR);
    static constexpr uint32 filterNearest = stringToHash(STR_NEAREST);
    static constexpr uint32 filterMipmapLinearLinear = stringToHash(STR_MIP_LINEAR_LINEAR);
    static constexpr uint32 filterMipmapLinearNearest = stringToHash(STR_MIP_LINEAR_NEAREST);
    static constexpr uint32 filterMipmapNearestLinear = stringToHash(STR_MIP_NEAREST_LINEAR );
    static constexpr uint32 filterMipmapNearestNearest = stringToHash(STR_MIP_NEAREST_NEAREST);

    //valid texture section param hashes
    static constexpr uint32 keyPath = stringToHash(STR_PARAM_PATH);
    static constexpr uint32 keyUWrap = stringToHash(STR_PARAM_U_WRAP);
    static constexpr uint32 keyVWrap = stringToHash(STR_PARAM_V_WRAP);
    static constexpr uint32 keyMinFilter = stringToHash(STR_PARAM_MIN_FILTER);
    static constexpr uint32 keyMagFilter = stringToHash(STR_PARAM_MAG_FILTER);

    // valid render queue options hashes
    static const uint32 renderQueueOpaque = stringToHash(STR_OPAQUE);
    static const uint32 renderQueueTranslucent = stringToHash(STR_TRANSLUCENT);
    static const uint32 renderQueueOverlay = stringToHash(STR_OVERLAY);

    //
    // Internal functions
    //

    inline Context* _context_get()
    {
      return &_context;
    }

#ifdef _LDK_DEBUG_
#define checkGlError() _checkNoGlError(__FILE__, __LINE__)

    static void _clearGlError()
    {
      GLenum err;
      do
      {
        err = glGetError();
      }while (err != GL_NO_ERROR);
    }

    static int32 _checkNoGlError(const char* file, uint32 line)
    {
      _clearGlError();
      const char* error = "UNKNOWN ERROR CODE";
      GLenum err = glGetError();
      int32 success = 1;
      uchar noerror = 1;
      while(err!=GL_NO_ERROR)
      {
        switch(err)
        {
          case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
          case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
          case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
          case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
          case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        success=0;
        LogError("GL ERROR %s at %s:%d",error, file, line);
        noerror=0;
        err=glGetError();
      }
      return success;
    }

#else
#define checkGlError() 
#endif

    static TextureWrap _cfgStringToTextureWrap(const char* cfgString)
    {
      uint32 paramHash = stringToHash(cfgString);
      uint32 cfgStringLen = strlen(cfgString);
      TextureWrap result = TextureWrap::REPEAT;

      if (paramHash == wrapRepeat 
          && strncmp(STR_REPEAT, cfgString, cfgStringLen) == 0)
        result = TextureWrap::REPEAT;
      else if (paramHash == wrapClampToEdge 
          && strncmp(STR_CLAMP_TO_EDGE, cfgString, cfgStringLen) == 0)
        result = TextureWrap::CLAMPTOEDGE;
      else if (paramHash == wrapMirroredRepeat 
          && strncmp(STR_MIRRORED_REPEAT, cfgString, cfgStringLen) == 0) 
        result = TextureWrap::MIRROREDREPEAT;
      else
        LogWarning("Invalid texture wrap mode '%s'. Defaulting to REPEAT", cfgString);

      return result;
    }

    static TextureFilter _cfgStringToTextureFilter(const char* cfgString)
    {
      uint32 paramHash = stringToHash(cfgString);
      uint32 cfgStringLen = strlen(cfgString);
      TextureFilter result = renderer::TextureFilter::LINEAR;

      if (paramHash == filterLinear 
          && strncmp(STR_LINEAR, cfgString, cfgStringLen) == 0)
        result = TextureFilter::LINEAR;
      else if (paramHash == filterNearest 
          && strncmp(STR_NEAREST, cfgString, cfgStringLen) == 0)
        result = TextureFilter::NEAREST;
      else if (paramHash == filterMipmapLinearLinear 
          && strncmp(STR_MIP_LINEAR_LINEAR, cfgString, cfgStringLen) == 0)
        result = TextureFilter::MIPMAPLINEARLINEAR;
      else if (paramHash == filterMipmapLinearNearest
          && strncmp(STR_MIP_LINEAR_NEAREST, cfgString, cfgStringLen) == 0)
        result = TextureFilter::MIPMAPLINEARNEAREST;
      else if (paramHash == filterMipmapNearestLinear
          && strncmp(STR_MIP_NEAREST_LINEAR, cfgString, cfgStringLen) == 0)
        result = TextureFilter::MIPMAPNEARESTLINEAR;
      else if (paramHash == filterMipmapNearestNearest
          && strncmp(STR_MIP_NEAREST_NEAREST, cfgString, cfgStringLen) == 0)
        result = TextureFilter::MIPMAPNEARESTNEAREST;
      else
        LogWarning("Invalid texture filter mode '%s'. Defaulting to LINEAR", cfgString);

      return result;
    }

    static uint32 _cfgStringToRenderQueue(const char* cfgString)
    {
      uint32 paramHash = stringToHash(cfgString);
      uint32 cfgStringLen = strlen(cfgString);
      uint32 result = RENDER_QUEUE_OPAQUE;

      if (paramHash == renderQueueOpaque 
          && strncmp(STR_OPAQUE, cfgString, cfgStringLen) == 0)
        result = RENDER_QUEUE_OPAQUE;
      else if (paramHash == renderQueueTranslucent 
          && strncmp(STR_TRANSLUCENT, cfgString, cfgStringLen) == 0)
        result = RENDER_QUEUE_TRANSLUCENT;
      else if (paramHash == renderQueueOverlay 
          && strncmp(STR_OVERLAY, cfgString, cfgStringLen) == 0)
        result = RENDER_QUEUE_OVERLAY;
      else
        LogWarning("Invalid render queue '%s'. Defaulting to OPAQUE", cfgString);

      return result;
    }

    static GLboolean _checkShaderProgramLink(GLuint program)
    {
      GLint success = 0;
      GLchar msgBuffer[1024] = {};
      glGetProgramiv(program, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(program, sizeof(msgBuffer), GL_FALSE, msgBuffer);
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

    static GLuint _internalToGlType(VertexAttributeType glType)
    {
      switch(glType)
      {
        case VertexAttributeType::FLOAT:
          return GL_FLOAT;
        case VertexAttributeType::BYTE:
          return GL_BYTE;
        case VertexAttributeType::UNSIGNED_BYTE:
          return GL_UNSIGNED_BYTE;
        case VertexAttributeType::SHORT:
          return GL_SHORT;
        case VertexAttributeType::UNSIGNED_SHORT:
          return GL_UNSIGNED_SHORT;
        case VertexAttributeType::INT:
          return GL_INT;
        case VertexAttributeType::UNSIGNED_INT:
          return GL_UNSIGNED_INT;
        case VertexAttributeType::DOUBLE:
          return GL_DOUBLE;
        case VertexAttributeType::SAMPLER:
          return GL_SAMPLER_2D;
        default:
          return GL_INVALID_ENUM;
      }
    }

    static GLuint _internalTypeSize(VertexAttributeType type)
    {
      switch(type)
      {
        case VertexAttributeType::FLOAT:
          return sizeof(GLfloat);
        case VertexAttributeType::BYTE:
          return sizeof(GLbyte);
        case VertexAttributeType::UNSIGNED_BYTE:
          return sizeof(GLubyte);
        case VertexAttributeType::SHORT:
          return sizeof(GLshort);
        case VertexAttributeType::UNSIGNED_SHORT:
          return sizeof(GLushort);
        case VertexAttributeType::INT:
        case VertexAttributeType::SAMPLER:
          return sizeof(GLint);
        case VertexAttributeType::BOOL:
          return sizeof(GLboolean);
        case VertexAttributeType::UNSIGNED_INT:
          return sizeof(GLuint);
        case VertexAttributeType::DOUBLE:
          return sizeof(GLdouble);
        default:
          LogWarning("Unknown vertex attribute type");
          return sizeof(GLfloat);
      }
    }

    static GLenum _internalToGlMinFilter(TextureFilter filter)
    {
      switch(filter)
      {
        case LINEAR:
          return GL_LINEAR;
        case NEAREST:
          return GL_NEAREST;
        case MIPMAPLINEARLINEAR:
          return GL_LINEAR_MIPMAP_LINEAR;
        case MIPMAPLINEARNEAREST:
          return GL_LINEAR_MIPMAP_NEAREST;
        case MIPMAPNEARESTLINEAR:
          return GL_NEAREST_MIPMAP_LINEAR;
        case MIPMAPNEARESTNEAREST:
          return GL_NEAREST_MIPMAP_NEAREST;
        default:
          LogError("Unknown TextureFilter");
          return GL_INVALID_ENUM;
      }
    }

    static GLenum _internalToGlMagFilter(TextureFilter filter)
    {
      switch(filter)
      {
        case LINEAR:
          return GL_LINEAR;
        case NEAREST:
          return GL_NEAREST;
        case MIPMAPLINEARLINEAR:
        case MIPMAPLINEARNEAREST:
        case MIPMAPNEARESTLINEAR:
        case MIPMAPNEARESTNEAREST:
          LogWarning("Invald TextureFilter for magFilter. Use LINEAR or NEAREST");
          return GL_NEAREST;
        default:
          LogError("Unknown TextureFilter");
          return GL_INVALID_ENUM;
      }
    }

    static GLenum _internalToGlWrap(TextureWrap wrap)
    {
      switch(wrap)
      {
        case CLAMPTOEDGE:
          return GL_CLAMP_TO_EDGE;
        case MIRROREDREPEAT:
          return GL_MIRRORED_REPEAT;
        case REPEAT:
          return GL_REPEAT;
        default:
          LogError("Unknown TextureWrap");
          return GL_INVALID_ENUM;
      }
    }

    static VertexAttributeType _glTypeToInternal(uint32 glType)
    {
      switch (glType)
      {
        case GL_FLOAT:
        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT4:
          return VertexAttributeType::FLOAT;

        case GL_BYTE:
          return VertexAttributeType::BYTE;

        case GL_UNSIGNED_BYTE:
          return VertexAttributeType::UNSIGNED_BYTE;

        case GL_SHORT:
          return VertexAttributeType::SHORT;

        case GL_UNSIGNED_SHORT:
          return VertexAttributeType::UNSIGNED_SHORT;

        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
          return VertexAttributeType::INT;

        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_VEC2:
        case GL_UNSIGNED_INT_VEC3:
        case GL_UNSIGNED_INT_VEC4:
          return VertexAttributeType::UNSIGNED_INT;

        case GL_BOOL:
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
          return VertexAttributeType::BOOL;

        case GL_DOUBLE:
        case GL_DOUBLE_VEC2:
        case GL_DOUBLE_VEC3:
        case GL_DOUBLE_VEC4:
        case GL_DOUBLE_MAT2:  
        case GL_DOUBLE_MAT3: 
        case GL_DOUBLE_MAT4:
        case GL_DOUBLE_MAT2x3:
        case GL_DOUBLE_MAT2x4:
        case GL_DOUBLE_MAT3x2:
        case GL_DOUBLE_MAT3x4:
        case GL_DOUBLE_MAT4x2:
        case GL_DOUBLE_MAT4x3:
          return VertexAttributeType::DOUBLE;

        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
          return VertexAttributeType::SAMPLER;

        default:
          return VertexAttributeType::UNKNOWN;
      }

    }

    static const Uniform* _findUniform(Shader* shader, char* name)
    {
      uint32 uniformCount = shader->uniformCount;
      int32 hash = stringToHash(name);

      for(int i=0; i < uniformCount; i++)
      {
        Uniform* uniform = &shader->uniforms[i];
        if (uniform->hash == hash)
          return uniform;
      }

      LogWarning("Could not find uniform '%s' on shader", name);
      return nullptr;
    }

    static void _setMatrix4(Material* material, char* name, ldk::Mat4* matrix, bool bindShader)
    {
      Shader* shader = &material->shader;
      const Uniform* uniform = _findUniform(shader, name);

      if(uniform)
      {
        LDK_ASSERT((uniform->type == ldk::renderer::VertexAttributeType::FLOAT
              && uniform->size == 1), "Mismatching uniform types");

        if(bindShader) 
          glUseProgram(shader->program);

        glUniformMatrix4fv(uniform->id, 1, 0, matrix->element);

        if(bindShader)
          glUseProgram(0);
      }
    }

    //TODO(marcio): Implement sorting here...
    static void _sortDrawCalls(DrawCall* calls, uint32 count) { }

    static void* _mapBuffer(Renderable* renderable, uint32 count)
    {
      //NOTE(marcio): This funciton exits with the current VBO bound
      VertexBuffer* buffer = &renderable->buffer;
      LDK_ASSERT(buffer->capacity >= count, "Buffer too small. Make buffer larger or draw less data.");

      uint32 dataEndOffset = renderable->index1 + count;

      if( dataEndOffset <= buffer->capacity)
      {
        renderable->index0 = renderable->index1;  // begin writting past last data written
        renderable->index1 = dataEndOffset;       // data written will spread this much
      }
      else
      {
        //TODO(marcio): Should usage be part of VertextBuffer instead of Renderable ?
        LDK_ASSERT(renderable->usage != GL_STATIC_DRAW, "Static buffers can not be overflown");

        // We are trying to write past the current buffer. Lets cycle buffers...
        ++renderable->currentVboIndex;
        renderable->currentVboIndex = renderable->currentVboIndex % renderable->vboCount;

        // make sure this buffer is note being used by GPU
        GLsync fence = renderable->fences[renderable->currentVboIndex];
        GLenum waitResult = glClientWaitSync(fence, 0, (uint64) 1000000000);
        LDK_ASSERT(waitResult != GL_TIMEOUT_EXPIRED, "We are GPU bound! Renderer is waiting for the gpu.");
        LDK_ASSERT(waitResult != GL_WAIT_FAILED, "Error waiting for sync object on GPU.");
        glDeleteSync(fence);

        // fix buffer chunk limits
        renderable->index0 = 0;
        renderable->index1 = count;
        renderable->needNewSync = 1;
      }

      // map the buffer with no driver syncrhonization
      uint32 vbo = renderable->vbos[renderable->currentVboIndex];
      glBindBuffer(GL_ARRAY_BUFFER, vbo);

      uint32 chunkStart = renderable->index0 * buffer->stride;
      uint32 chunkSize = (renderable->index1 - renderable->index0) * buffer->stride;
      void* memory = glMapBufferRange(GL_ARRAY_BUFFER, chunkStart, chunkSize, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
      checkGlError();
      LDK_ASSERT(memory, "glMapBufferRange returned null");
      return memory;
    }

    static void _unmapBuffer()
    {
      glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    // Send vertex data to the GPU
    static void _uploadVertexData(DrawCall* drawCall)
    {
      uint32 vertexCount = drawCall->vertexCount;
      Renderable* renderable = drawCall->renderable; 
      VertexBuffer* buffer = &drawCall->renderable->buffer;

      //TODO(marcio): check if glBufferSubData is faster than mapping/coppying
      void* driverMemory = _mapBuffer(renderable, vertexCount);
      memcpy(driverMemory, drawCall->vertices, buffer->stride * vertexCount);
      _unmapBuffer();
    }

    static void _executeDrawCall(Mat4& projectionMatrix, DrawCall* drawCall) 
    {
      // if there is no pointer to a renderable, try fetching from a handle
      Renderable* renderable = drawCall->renderable; 

      // apply the material to the renderable
      renderable_setMaterial(drawCall->renderable, drawCall->material);
      Material* material = (Material*) ldkEngine::handle_getData(drawCall->renderable->materialHandle.handle);

      uint32 currentVboIndex = renderable->currentVboIndex;
      uint32 vbo = renderable->vbos[currentVboIndex];

      if(renderable->usage == GL_STATIC_DRAW)
      {
        if (renderable->needNewSync)
        {
          renderable->needNewSync = 0; 
          _uploadVertexData(drawCall);
        }
      }
      else
      {
        _uploadVertexData(drawCall);
      }

      VertexBuffer* buffer = &(renderable->buffer);
      glUseProgram(material->shader.program);
      checkGlError();

      // send projection matrix
      _setMatrix4(material, "mprojection", &projectionMatrix, false);
      // send model matrix
      _setMatrix4(material, "mmodel", &renderable->modelMatrix, false);
      checkGlError();

      // Set buffer format
      glBindBuffer(GL_ARRAY_BUFFER, vbo); // if we didn't sync, the vbo might not be bound yet
      uint32 vertexStride = buffer->stride;
      uint32 attributeCount = buffer->attributeCount;
      for (int i = 0; i < attributeCount; ++i) 
      {
        VertexAttribute* attribute = buffer->attributes + i;    
        glEnableVertexAttribArray(attribute->location);
        GLuint glType = _internalToGlType(attribute->type);
        glVertexAttribPointer(attribute->location, attribute->size, glType, GL_FALSE, vertexStride,
            (void*)((size_t) attribute->offset));
      }

      // enable/bind textures
      uint32 textureCount = material->textureCount;
      for (int i = 0; i < textureCount; ++i) 
      {
        glActiveTexture(GL_TEXTURE0 + i);
        checkGlError();
        uint32 textureId = material->texture[i].id;
        glBindTexture(GL_TEXTURE_2D, textureId);
        checkGlError();
      }

      //TODO(marcio): implement instanced rendering
      switch (drawCall->type) 
      {
        case DrawCall::DRAW:
          {
            uint32 streamStart = renderable->index0;
            uint32 streamSize = renderable->index1 - streamStart;
            glDrawArrays(buffer->primitive, streamStart, streamSize);
          }
          break;
        case DrawCall::DRAW_INDEXED:
          {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->ibo);
            glDrawElements(buffer->primitive,
                drawCall->indexCount, 
                GL_UNSIGNED_INT,
                (const char*)(drawCall->indexStart * sizeof(uint32)));
          }
          break;
        default:
          LDK_ASSERT(false, "Uknown render command passed to renderer");
          break;
      }

      // create new fence if necessary
      if (renderable->needNewSync)
      {
        // TODO: This shouldn't be called for static buffers
        renderable->fences[currentVboIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        renderable->needNewSync = 0;
      }

      // disable bound attributes
      for (int i = 0; i < attributeCount; i++) 
      {
        VertexAttribute* attribute = buffer->attributes + 1;    
        glDisableVertexAttribArray(attribute->location);
      }

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glUseProgram(0);
    }

    //
    // Shader functions
    //

    static bool loadShader(Shader* shader, char* vertexSource, char* fragmentSource)
    {
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

      LDK_ASSERT(uniformCount <= LDK_GL_MAX_UNIFORM_COUNT, "Too many uniforms");

      uint32 maxUniformNameLength = sizeof(char) * LDK_GL_UNIFORM_NAME_LENGTH;

      for (int i = 0; i < uniformCount; i++) 
      {
        uint32 nameLength;
        Uniform uniform = {};

        glGetActiveUniform(program, (GLint)i, maxUniformNameLength, (GLsizei*) &nameLength, (GLsizei*)&uniform.size,
            (GLenum*)&uniform.type, (GLchar*)&uniform.name);

        uniform.location = glGetUniformLocation(program, uniform.name);
        uniform.id = i;
        uniform.hash = stringToHash(uniform.name);
        uniform.type = _glTypeToInternal(uniform.type);
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

    bool makeMaterial(Material* material, char* vertexSource, char* fragmentSource, uint32 renderQueue)
    {
      material->textureCount = 0;
      material->renderQueue = renderQueue;
      return loadShader(&material->shader, vertexSource, fragmentSource);
    }

    void material_setMatrix4(HMaterial materialHandle, char* name, ldk::Mat4* matrix)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      _setMatrix4(material, name, matrix, true);
    }

    void material_setInt(HMaterial materialHandle, char* name, uint32 intParam)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      Shader* shader = &material->shader;
      glUseProgram(shader->program);
      const Uniform* uniform = _findUniform(shader, name);
      glUniform1i(uniform->location, intParam);
      checkGlError();
    }

    void material_setInt(HMaterial materialHandle, char* name, uint32 count, uint32* intParam)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      Shader* shader = &material->shader;
      glUseProgram(shader->program);
      checkGlError();

      const Uniform* uniform = _findUniform(shader, name);

      switch(count)
      {
        case 1:
          glUniform1i(uniform->location, intParam[0]);
          break;

        case 2:
          glUniform2i(uniform->location, intParam[0], intParam[1]);
          break;

        case 3:
          glUniform3i(uniform->location, intParam[0], intParam[1], intParam[2]);
          break;

        case 4:
          glUniform4i(uniform->location, intParam[0], intParam[1], intParam[2], intParam[3]);
          break;

        default:
          LDK_ASSERT(count > 0 && count < 4, "setShaderInt count is between 0 and 4");
          break;
      }

      checkGlError();
      glUseProgram(0);
    }

    void material_setFloat(HMaterial materialHandle, char* name, float floatParam)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      Shader* shader = &material->shader;
      glUseProgram(shader->program);
      const Uniform* uniform = _findUniform(shader, name);
      glUniform1f(uniform->location, floatParam);
      checkGlError();
    }

    void material_setFloat(HMaterial materialHandle, char* name, uint32 count, float* floatParam)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      Shader* shader = &material->shader;
      glUseProgram(shader->program);
      const Uniform* uniform = _findUniform(shader, name);

      switch(count)
      {
        case 1:
          glUniform1f(uniform->location, floatParam[0]);
          break;

        case 2:
          glUniform2f(uniform->location, floatParam[0], floatParam[1]);
          break;

        case 3:
          glUniform3f(uniform->location, floatParam[0], floatParam[1], floatParam[2]);
          break;

        case 4:
          glUniform4f(uniform->location, floatParam[0], floatParam[1], floatParam[2], floatParam[3]);
          break;

        default:
          LDK_ASSERT(count > 0 && count < 4, "setShaderFloat count is between 0 and 4");
          break;
      }
      glUseProgram(0);
      checkGlError();
    }

    bool material_setTexture(HMaterial materialHandle, char* name, Texture texture)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      // Find the thexture slot or allocate a new one if this is a new texture
      uint32 textureSlot = -1;
      for(int i=0; i < material->textureCount; i++)
      {
        if (texture.id == material->texture[i].id)
        {
          textureSlot = i;
          break;
        }
      }

      if (textureSlot == -1)
      {
        if (LDK_GL_MAX_TEXTURES <= material->textureCount) 
          return false;

        textureSlot = material->textureCount;
        material->texture[textureSlot] = texture;
        material->textureCount++;
      }

      material_setInt(materialHandle, name, textureSlot); 
      return true;
    }

    void renderable_setMaterial(Renderable* renderable, HMaterial materialHandle)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      if (renderable->materialHandle.handle == materialHandle.handle)
      {
        return;
      }

      renderable->materialHandle = materialHandle;
      Shader* shader = &material->shader;

      glGetProgramiv(shader->program, GL_ACTIVE_ATTRIBUTES, (GLint*) &renderable->attributeCount);

      if(renderable->attributeCount != renderable->buffer.attributeCount)
      {
        LogWarning("Shader and buffer layout has different attribute count %d/%d", renderable->attributeCount, renderable->buffer.attributeCount);
      }

      uint32 attribType = 0;
      uint32 attribSize = 0;
      uint64 attribHash = 0;
      char attribName[256];

      int32 attribCount = renderable->buffer.attributeCount;
      for (int i = 0; i < attribCount; i++) 
      {
        // query attribute type and size
        glGetActiveAttrib(shader->program, i, 256, 0, (GLint*) &attribSize,  (GLenum*) &attribType, (GLchar*)&attribName);
        attribHash = ldk::stringToHash(attribName);
        VertexAttribute* attribute;

        for (int j = 0; j < attribCount; j++) 
        {
          VertexAttribute* tmpAttribute = renderable->buffer.attributes + j;
          if(tmpAttribute->hash == attribHash)
          {
            attribute = tmpAttribute;
            break;
          }
        }

        LDK_ASSERT(attribute, "No matching attribute found");
        LDK_ASSERT(attribute->type == _glTypeToInternal(attribType), "No matching attribute type");
        attribute->location = glGetAttribLocation(shader->program, attribName);
        checkGlError();
      }
    }

    //
    // Context functions
    //

    void context_setClearColor(const Vec4& color)
    {
      Context* context = _context_get();
      LDK_ASSERT_VALID_CONTEXT(context);
      context->clearColor = color;
      glClearColor(color.x, color.y, color.z, color.w);
    }

    void context_initialize(uint32 maxDrawCalls, const Vec4& clearColor, uint32 settingsBits)
    {
      //TODO(marcio): Review if we are gonna stick to this settingsBits param. Seems useless now.
      Context* context = _context_get();
      context->settingsBits = settingsBits;
      context->maxDrawCalls = maxDrawCalls;
      context->drawCallCount = 0;
      context->drawCalls = (ldk::renderer::DrawCall*) 
        ldkEngine::memory_alloc(sizeof(ldk::renderer::DrawCall) * maxDrawCalls, ldkEngine::Allocation::Tag::RENDERER);

      if(!context->drawCalls)
      {
        ldkEngine::memory_free(context);
        return;
      }

      GLuint vao;
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      context->initialized = true;

      context_setClearColor(clearColor);
    }

    void renderer::clearBuffers(uint32 clearBits)
    {
      glClear(clearBits);
      checkGlError();
    }

    void context_finalize()
    {
      Context* context = _context_get();
      LDK_ASSERT_VALID_CONTEXT(context);
      ldkEngine::memory_free(context->drawCalls);
    }

    //
    // Buffer functions
    //
    void makeVertexBuffer(VertexBuffer* buffer, uint32 vertexCount) 
    {
      buffer->capacity = vertexCount;
      buffer->stride = 0;
      buffer->primitive = GL_TRIANGLES; // only primitive supported by now
      buffer->attributeCount = 0;
    }

    void addVertexBufferAttribute(VertexBuffer* buffer, char* name, uint32 size, VertexAttributeType type, uint32 offset)
    {
      LDK_ASSERT((buffer->attributeCount < LDK_GL_MAX_VERTEX_ATTRIBUTES),
          "Maximum attribute count reached for buffer layout");

      VertexAttribute attribute;
      attribute.hash = ldk::stringToHash(name);
      attribute.size = size;
      attribute.offset = offset;
      attribute.type = type;
      attribute.name = name;

      int32 index = buffer->attributeCount++;

      size_t typeSize = _internalTypeSize(type);
      buffer->stride += size * typeSize;
      VertexAttribute* attributeSlot = &buffer->attributes[index];
      *attributeSlot = attribute;
    }

    void makeRenderable(Renderable* renderable, VertexBuffer* vertexBuffer, bool isStatic)
    {
      *renderable = {};
      renderable->buffer = *vertexBuffer;

      if (isStatic)
      {
        renderable->usage = GL_STATIC_DRAW;
        renderable->vboCount = 1;
        renderable->needNewSync = 1;
      }
      else
      {
        renderable->usage = GL_DYNAMIC_DRAW;
        renderable->vboCount = LDK_GL_NUM_VBOS;
      }

      VertexBuffer* buffer = &renderable->buffer;
      for (int i = 0; i < renderable->vboCount; i++) 
      {
        GLuint* vbo = renderable->vbos + i;
        glGenBuffers(1, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer->capacity * buffer->stride, NULL, renderable->usage);
        renderable->fences[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void makeRenderable(Renderable* renderable, VertexBuffer* vertexBuffer, uint32* indices, uint32 maxIndexCount, bool isStatic)
    {
      makeRenderable(renderable, vertexBuffer, isStatic);

      GLuint ibo;
      uint32 iboSize = maxIndexCount * sizeof(uint32);

      //NOTE: Index buffers are always static, by now... Do we need it to be dynamic ?
      glGenBuffers(1, &ibo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, indices, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      renderable->ibo = ibo;
      renderable->iboSize = iboSize;
      checkGlError();
    }

    ldk::HRenderable renderable_create(ldk::HMesh meshHandle, ldk::HMaterial materialHandle)
    {
      size_t totalSize = sizeof(ldk::renderer::Renderable) + sizeof(ldk::renderer::VertexBuffer);
      char8* mem = (char8*) ldkEngine::memory_alloc(totalSize, ldkEngine::Allocation::Tag::RENDERER);

      ldk::renderer::Renderable* renderable = (ldk::renderer::Renderable*) mem;

      ldk::renderer::VertexBuffer* vertexBuffer =
        (ldk::renderer::VertexBuffer*) (mem + sizeof(ldk::renderer::Renderable));

      ldk::Mesh* mesh = (ldk::Mesh*) ldkEngine::handle_getData(meshHandle.handle);
      ldk::MeshInfo* meshInfo = &(mesh->meshData->info);

      if(mesh->meshData->info.format == ldk::MeshInfo::VertexFormat::PNUV)
      {
        // Set buffer layout for PNUV mesh
        renderer::makeVertexBuffer(vertexBuffer, meshInfo->vertexCount);
        renderer::addVertexBufferAttribute(vertexBuffer, "_pos", 3,
            renderer::VertexAttributeType::FLOAT, 0);

        renderer::addVertexBufferAttribute(vertexBuffer, "_normal", 3, 
            renderer::VertexAttributeType::FLOAT, 3 * sizeof(float));

        renderer::addVertexBufferAttribute(vertexBuffer, "_uv", 2,
            renderer::VertexAttributeType::FLOAT, 6 * sizeof(float));
      }
      else
      {
        LDK_ASSERT(false, "Unknown vertex format on mesh");
        return typedHandle_invalid<HRenderable>();
      }

      // Initialize an indexed vertex buffer
      makeRenderable(renderable, vertexBuffer, mesh->indices, meshInfo->indexCount, true);

      // Set material to renderable
      renderer::renderable_setMaterial(renderable, materialHandle);

      renderable->meshHandle = meshHandle;
      ldk::Handle renderableHandle =
        ldkEngine::handle_store(ldkEngine::HandleType::RENDERABLE, (void*)renderable);
      return typedHandle_make<HRenderable>(renderableHandle);
    }

    void renderable_setMatrix(ldk::HRenderable renderableHandle, const Mat4* modelMatrix)
    {
      renderer::Renderable* renderable =
        (renderer::Renderable*) ldkEngine::handle_getData(renderableHandle.handle);
      renderable->modelMatrix = *modelMatrix;
    }

    //
    // Render functions
    //

    void beginFrame(Mat4& projection)
    {
      Context* context = _context_get();
      context->projectionMatrix = projection;
    }

    void setViewPort(Rect& viewPort)
    {
      glViewport(viewPort.x, viewPort.y, viewPort.w, viewPort.h);
    }

    void pushDrawCall(DrawCall* drawCall)
    {
      Context* context = _context_get();
      LDK_ASSERT_VALID_CONTEXT(context);
      LDK_ASSERT(context->drawCallCount < context->maxDrawCalls,
          "Exceeded maximum draw calls per frame at current context");
      context->drawCalls[context->drawCallCount++] = *drawCall;
    }

    void drawIndexed(ldk::HRenderable renderableHandle)
    {
      Context* context = _context_get();
      LDK_ASSERT_VALID_CONTEXT(context);

      ldk::renderer::Renderable* renderable = 
        (ldk::renderer::Renderable*) ldkEngine::handle_getData(renderableHandle.handle);
      ldk::Mesh* mesh = (ldk::Mesh*) ldkEngine::handle_getData(renderable->meshHandle.handle);
      ldk::MeshInfo* meshInfo = &mesh->meshData->info;

      ldk::renderer::DrawCall drawCall;
      drawCall.renderable = renderable;
      drawCall.type = renderer::DrawCall::DRAW_INDEXED;
      drawCall.vertexCount = meshInfo->vertexCount;
      drawCall.vertices = (void*)mesh->vertices;
      drawCall.indexStart = 0;
      drawCall.indexCount = meshInfo->indexCount;
      drawCall.material = renderable->materialHandle;
      pushDrawCall(&drawCall);
    }

    void endFrame()
    {
      Context* context = _context_get();
      LDK_ASSERT_VALID_CONTEXT(context);

      uint32 drawCallCount = context->drawCallCount;
      _sortDrawCalls(context->drawCalls, drawCallCount);

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glDepthFunc(GL_LESS);

      // execute draw calls
      for (int i = 0; i < drawCallCount; i++) 
      {
        DrawCall* drawCall = context->drawCalls + i;
        _executeDrawCall(context->projectionMatrix, drawCall);
      }

      // reset renderable indices
      for (int i = 0; i < drawCallCount; i++) 
      {
        DrawCall* drawCall = context->drawCalls + i;
        Renderable* renderable = drawCall->renderable;
        renderable->index0 = 0;
        renderable->index1 = 0;
      }

      // reset draw call count for this frame
      context->drawCallCount = 0;
      checkGlError();
    }

    Texture createTexture(ldk::HBitmap bmpHandle
        ,TextureFilter minFilter 
        ,TextureFilter magFilter
        ,TextureWrap uWrap
        ,TextureWrap vWrap)
    {
      GLuint textureId;
      glGenTextures(1, &textureId);
      glBindTexture(GL_TEXTURE_2D, textureId);

      ldk::Bitmap* bitmap = (ldk::Bitmap*) ldkEngine::handle_getData(bmpHandle.handle);
      if(bitmap == nullptr)
      {
        bitmap = getPlaceholderBmp();
      }

      GLenum fmt = GL_RGBA;
      GLenum pixelDataFmt = GL_UNSIGNED_BYTE;

      if (bitmap->bitsPerPixel == 24)
        fmt = GL_RGB;
      else if (bitmap->bitsPerPixel == 16)
      {
        fmt = GL_RGBA;
        pixelDataFmt = GL_UNSIGNED_SHORT_1_5_5_5_REV;
      }


      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width, bitmap->height,
          0, fmt, pixelDataFmt, bitmap->pixels);

      checkGlError();
      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _internalToGlMinFilter(minFilter));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _internalToGlMagFilter(magFilter));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _internalToGlWrap(uWrap));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _internalToGlWrap(vWrap));
      glBindTexture(GL_TEXTURE_2D, 0);
      checkGlError();

      Texture texture;
      texture.id = textureId;
      texture.width = bitmap->width;
      texture.height = bitmap->height;
      return texture;
    }

    void destroyTexture(Texture& texture)
    {
      glBindTexture(GL_TEXTURE_2D, 0);
      glDeleteTextures(1, &texture.id);
      texture.id = 0;
      texture.width = texture.height = 0;
      checkGlError();
    }

    ldk::HMaterial loadMaterial(const char* file)
    {
      auto cfgRoot = ldk::configParseFile(file);
      auto materialSection = ldk::configGetSection(cfgRoot, "material");

      char* path;
      char* fs;
      size_t shaderFileSize = 0;
      if (!ldk::configGetString(materialSection, (const char*) "frag-shader", &path))
      {
        LogError("Error loading material. Missing Fragment shader.");
        ldk::configDispose(cfgRoot);
        return typedHandle_invalid<HMaterial>();
      }

      // null terminate the file content
      fs = (char*) ldk::platform::loadFileToBufferOffset(path, &shaderFileSize , 1, 0);
      char* eoBuffer = fs + shaderFileSize;
      *eoBuffer = 0;

      char* vs;
      if(!ldk::configGetString(materialSection, (const char*) "vert-shader", &path))
      {
        LogError("Error loading material. Missing Vertex shader.");
        ldk::configDispose(cfgRoot);
        return typedHandle_invalid<HMaterial>();
      }

      // null terminate the file content
      vs = (char*) ldk::platform::loadFileToBufferOffset(path, &shaderFileSize, 1, 0);
      eoBuffer = vs + shaderFileSize;
      *eoBuffer = 0;

      uint32 renderQueue = (uint32) RENDER_QUEUE_OPAQUE;
      char* temp;
      if(ldk::configGetString(materialSection, (const char*) "render-queue", &temp))
      {
        renderQueue = (uint32) _cfgStringToRenderQueue(temp);
        int32 queueOffset = 0;
        configGetInt(materialSection, "render-queue-offset", &queueOffset);
        renderQueue += queueOffset;
      }

      // allocate a material
      ldk::renderer::Material* material = 
        (ldk::renderer::Material*) ldkEngine::memory_alloc(sizeof(ldk::renderer::Material), ldkEngine::Allocation::MATERIAL);
      ldk::Handle handle = ldkEngine::handle_store(ldkEngine::HandleType::MATERIAL, material);

      // Store it in the handle table
      if (handle == ldkEngine::handle_invalid())
      {
        ldkEngine::memory_free(material);
        ldk::platform::memoryFree(material);
        return typedHandle_invalid<HMaterial>();
      }
      HMaterial materialHandle = typedHandle_make<HMaterial>(handle);

      makeMaterial(material, vs, fs, renderQueue);
      ldk::platform::memoryFree(fs);
      ldk::platform::memoryFree(vs);

      TextureWrap uWrap;
      TextureWrap vWrap;
      TextureFilter minFilter;
      TextureFilter magFilter;
      char* texturePath = nullptr;

      // Parse texture sections
      auto section = ldk::configGetFirstSection(cfgRoot);
      while (section != nullptr)
      {
        if (section->hash == texture0 ||section->hash == texture1 
            ||section->hash == texture2 ||section->hash == texture3
            ||section->hash == texture4 ||section->hash == texture5 
            ||section->hash == texture6 ||section->hash == texture7)
        {
          auto variant = ldk::configGetFirstVariant(section);
          while  (variant != nullptr)
          {

            const uint32 keyLen = strlen(variant->key);
            char *value;

            if(variant->hash == keyUWrap
                && strncmp(STR_PARAM_U_WRAP, variant->key, keyLen) == 0)
            {
              configGetString(section, (const char*) variant, &value);
              uWrap = _cfgStringToTextureWrap(value);
            }
            else if (variant->hash == keyVWrap
                && strncmp(STR_PARAM_V_WRAP, variant->key, keyLen) == 0)
            {
              configGetString(section, (const char*) variant, &value);
              vWrap = _cfgStringToTextureWrap(value);
            }
            else if(variant->hash == keyPath
                && strncmp(STR_PARAM_PATH, variant->key, keyLen) == 0)
            {
              configGetString(section, (const char*) variant, &texturePath);
            }
            else if (variant->hash == keyMinFilter
                && strncmp(STR_PARAM_MIN_FILTER, variant->key, keyLen) == 0)
            {
              configGetString(section, (const char*) variant, &value);
              minFilter = _cfgStringToTextureFilter(value);
            }
            else if (variant->hash == keyMagFilter
                && strncmp(STR_PARAM_MAG_FILTER, variant->key, keyLen) == 0)
            {
              configGetString(section, (const char*) variant, &value);
              magFilter = _cfgStringToTextureFilter(value);
            }
            else
            {
              // ignore if not a valid texture parameter
              LogWarning("Ignoring unknown texture parameter '%s' for texture '%s'",
                  variant->key, section->name);
              continue;
            }

            variant = configGetNextVariant(section, variant);
          } // whie variant...

          // create the texture based on the parsed parameters
          if(texturePath != nullptr)
          {
            bool isPlaceholder = false;
            ldk::HBitmap bmpHandle = ldk::asset_loadBitmap((const char*)texturePath);
            Texture texture = renderer::createTexture(bmpHandle, minFilter, magFilter, uWrap, vWrap);
            asset_unload(bmpHandle);
            renderer::material_setTexture(materialHandle, (char*) &section->name, texture);
          }
          else
          {
            LogWarning("%s has no path and will be ignored.",
                section->name);
          }
        }

        section = configGetNextSection(cfgRoot, section);
      } // while sections ...

      ldk::configDispose(cfgRoot);

      return materialHandle;
    }

    void material_destroy(ldk::HMaterial materialHandle)
    {
      Material* material = (Material*) ldkEngine::handle_getData(materialHandle.handle);
      LDK_ASSERT(material != nullptr, "Unexpected null pointer to asset");

      // unload textures from GPU
      for(uint32 i = 0; i < material->textureCount; i++)
      {
        Texture& texture = material->texture[i];
        destroyTexture(texture);
        checkGlError();
      }

      // unload shader from GPU
      glDeleteProgram(material->shader.program);
      checkGlError();

      platform::memoryFree(material);
      //TOD(marcio): Should we delete the actual BITMAP from RAM ? Review when asset manager is done!
    }

    void renderable_destroy(ldk::HRenderable renderableHandle)
    {
      //NOTE:this does not unload the associated mesh
      ldk::renderer::Renderable* renderable = 
        (ldk::renderer::Renderable*) ldkEngine::handle_getData(renderableHandle.handle);
      glDeleteBuffers((GLsizei)renderable->vboCount, &(renderable->vbos[0]));
      glDeleteBuffers((GLsizei)1, &renderable->ibo);
      ldkEngine::handle_remove(renderableHandle.handle);
      ldkEngine::memory_free(renderable);
    }
  } // renderer
} // ldk

#ifdef LDK_ASSERT_VALID_CONTEXT
#undef LDK_ASSERT_VALID_CONTEXT
#endif
