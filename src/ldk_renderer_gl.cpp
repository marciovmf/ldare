
#define LDK_GL_ALLOC(size) malloc((size))
#define LDK_GL_FREE(ptr) free((ptr))

namespace ldk
{
  namespace gl
  {

#ifdef _LDK_DEBUG_
#define checkGlError() checkNoGlError(__FILE__, __LINE__)
#else
#define checkGlError() 
#endif

    static int32 checkNoGlError(const char* file, uint32 line)
    {
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

    static void clearGlError()
    {
      GLenum err;
      do
      {
        err = glGetError();
      }while (err != GL_NO_ERROR);
    }

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

    static GLuint _internalToGlType(uint32 glType)
    {
      switch(glType)
      {
        case VertexAttributeType::INT:
          return GL_INT;
        case VertexAttributeType::FLOAT:
          return GL_FLOAT;
        case VertexAttributeType::BOOL:
          return GL_BOOL;
        case VertexAttributeType::SAMPLER:
          return GL_SAMPLER;
        case VertexAttributeType::UNKNOWN:
        default:
          return GL_INVALID_ENUM;
      }
    }

    static VertexAttributeType _glTypeToInternal(uint32 glType)
    {
      switch (glType)
      {
        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
          return VertexAttributeType::INT;

        case GL_FLOAT:
        case GL_FLOAT_VEC2:
        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT4:
          return VertexAttributeType::FLOAT;

        case GL_BOOL:
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
          return VertexAttributeType::BOOL;

        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
          return VertexAttributeType::SAMPLER;

        default:
          return VertexAttributeType::UNKNOWN;
      }

    }

    //TODO(marcio): Implement sorting here...
    static void _sortDrawCalls(DrawCall* calls, uint32 count) { }

    static void* _mapBuffer(Renderable* renderable, uint32 count)
    {
      VertexBuffer* buffer = &renderable->buffer;
      LDK_ASSERT(buffer->size >= count, "Buffer too small. Make buffer larger or draw less data.");

      uint32 dataEndOffset = renderable->index1 + count;

      if( dataEndOffset <= buffer->size)
      {
        renderable->index0 = renderable->index1;  // begin writting past last data written
        renderable->index1 = dataEndOffset;       // data written will spread this much
      }
      else
      {
        //TODO(marcio): Should usage be part of VertextBuffer instead of
        //Renderable ?
        LDK_ASSERT(renderable->usage != GL_STATIC_DRAW, "Static buffers can not be overflown");

        // We are trying to write past the current buffer. Lets cycle buffers...
        ++renderable->currentVboIndex;
        renderable->currentVboIndex = renderable->currentVboIndex & renderable->vboCount;
      
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

      //TODO: marcio, map/unmap a buffer only ONCE per flush.
      uint32 chunkStart = renderable->index0 * buffer->stride;
      uint32 chunkSize = (renderable->index1 - renderable->index0) * buffer->stride;
      void* memory = glMapBufferRange(GL_ARRAY_BUFFER, chunkStart, chunkSize, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

      LDK_ASSERT(memory, "glMapBufferRange returned null");
      return memory;
    }

    static void _unmapBuffer()
    {
      glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    // Send vertex data to the GPU
    // gl_do_map_internal
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

    static void _executeDrawCall(DrawCall* drawCall)
    {
      Renderable* renderable = drawCall->renderable; 
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

      clearGlError();
      VertexBuffer* buffer = &(renderable->buffer);
      glUseProgram(renderable->shader->program);
      checkGlError();

      uint32 currentVboIndex = renderable->currentVboIndex;
      uint32 vbo = renderable->vbos[currentVboIndex];
      glBindBuffer(GL_ARRAY_BUFFER, vbo);

      // Set buffer format
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

      clearGlError();
      // enable/bind textures
      uint32 textureCount = drawCall->textureCount;
      for (int i = 0; i < textureCount; ++i) 
      {
        glActiveTexture(GL_TEXTURE0 + i) ;
        checkGlError();
        uint32 textureId = drawCall->textureId[i];
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
            glDrawElements(buffer->primitive, drawCall->indexCount, GL_UNSIGNED_INT, (const GLvoid*)drawCall->indexStart);
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

    //TODO:We need a way to add textures to a material. This is how I think i
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

    void setShaderMatrix4(Shader* shader, char* name, ldk::Mat4* matrix)
    {
      const Uniform* uniform = _findUniform(shader, name);
      
      if(uniform)
      {
        LDK_ASSERT((uniform->type == ldk::gl::VertexAttributeType::FLOAT
           && uniform->size == 1), "Mismatching uniform types");

        glUseProgram(shader->program);
        glUniformMatrix4fv(uniform->id, 1, 0, matrix->element);
        glUseProgram(0);
      }
    }

    void setShaderInt(Shader* shader, char* name, uint32 count, uint32* intParam)
    {
      clearGlError();
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

    void setShaderTexture(Shader* shader, char* name, const int32* textureId)
    {
      setShaderInt(shader, name, 1, (uint32*) textureId);
    }

    void setShaderFloat(Shader* shader, char* name, uint32 count, float* floatParam)
    {
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
    }

    void setShader(Renderable* renderable, Shader* shader)
    {
      renderable->shader = shader;
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
        // cache attribute location
        attribute->location = glGetAttribLocation(shader->program, attribName);
      }

      VertexBuffer* buffer = &renderable->buffer;
      for (int i = 0; i < renderable->vboCount; i++) 
      {
        GLuint* vbo = renderable->vbos + i;
        glGenBuffers(1, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer->size * buffer->stride, NULL, renderable->usage);
        renderable->fences[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    //
    // Context functions
    //

    Context* createContext(uint32 maxDrawCalls, uint32 clearBits, uint32 settingsBits)
    {
      Context* context = (Context*) LDK_GL_ALLOC(sizeof(Context));
      if (!context) return nullptr;

      context->clearBits = clearBits;
      context->settingsBits = settingsBits;
      context->maxDrawCalls = maxDrawCalls;
      context->drawCallCount = 0;
      context->drawCalls = (ldk::gl::DrawCall*) LDK_GL_ALLOC(sizeof(ldk::gl::DrawCall) * maxDrawCalls);

      if(!context->drawCalls)
      {
        LDK_GL_FREE(context);
        return nullptr;
      }

      GLuint vao;
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      return context;
    }

    void destroyContext(Context* context)
    {
        LDK_GL_FREE(context->drawCalls);
        LDK_GL_FREE(context);
    }

    //
    // Buffer functions
    //

    void makeVertexBuffer(VertexBuffer* buffer, uint32 bufferSize, uint32 stride) 
    {
      buffer->size = bufferSize;
      buffer->stride = stride;
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
    }

    //
    // Render functions
    //
    void pushDrawCall(Context* context, DrawCall* drawCall)
    {
      LDK_ASSERT(context->drawCallCount < context->maxDrawCalls, "Exceeded maximum draw calls per frame at current context");
      context->drawCalls[context->drawCallCount++] = *drawCall;
    }

    void flush(Context* context)
    {
      uint32 drawCallCount = context->drawCallCount;
      _sortDrawCalls(context->drawCalls, drawCallCount);
      
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glDepthFunc(GL_LESS);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // execute draw calls
      for (int i = 0; i < drawCallCount; i++) 
      {
        DrawCall* drawCall = context->drawCalls + i;
        _executeDrawCall(drawCall);
      }

      // reset draw call count for this frame
      context->drawCallCount = 0;
    }

    int32 createTexture(const Bitmap* bitmap)
    {
      GLuint textureId;
      glGenTextures(1, &textureId);
      clearGlError();
      glBindTexture(GL_TEXTURE_2D, textureId);
      checkGlError();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      clearGlError();
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width, bitmap->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels);
      checkGlError();
      glBindTexture(GL_TEXTURE_2D, 0);
      checkGlError();
      return textureId;
    }


  } // gl
} // ldk
