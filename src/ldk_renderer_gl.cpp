

#define LDK_GL_ALLOC(size) malloc((size))
#define LDK_GL_FREE(ptr) free((ptr))

namespace ldk
{
  namespace renderer
  {
    //
    // Internal functions
    //

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

    //TODO(marcio): Implement sorting here...
    static void _sortDrawCalls(DrawCall* calls, uint32 count) { }

    static void* _mapBuffer(Renderable* renderable, uint32 count)
    {
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

      VertexBuffer* buffer = &(renderable->buffer);
      glUseProgram(renderable->material->shader.program);
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

      // enable/bind textures
      Material& material = *drawCall->renderable->material;
      uint32 textureCount = material.textureCount;
      for (int i = 0; i < textureCount; ++i) 
      {
        glActiveTexture(GL_TEXTURE0 + i);
        checkGlError();
        uint32 textureId = material.texture[i].id;
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
                (const char*)nullptr + drawCall->indexStart);
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

    void setMatrix4(Material* material, char* name, ldk::Mat4* matrix)
    {
      Shader* shader = &material->shader;
      const Uniform* uniform = _findUniform(shader, name);

      if(uniform)
      {
        LDK_ASSERT((uniform->type == ldk::renderer::VertexAttributeType::FLOAT
              && uniform->size == 1), "Mismatching uniform types");

        glUseProgram(shader->program);
        glUniformMatrix4fv(uniform->id, 1, 0, matrix->element);
        glUseProgram(0);
      }
    }

    void setInt(Material* material, char* name, uint32 intParam)
    {
      Shader* shader = &material->shader;
      glUseProgram(shader->program);
      const Uniform* uniform = _findUniform(shader, name);
      glUniform1i(uniform->location, intParam);
      checkGlError();
    }

    void setInt(Material* material, char* name, uint32 count, uint32* intParam)
    {
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

    void setFloat(Material* material, char* name, float floatParam)
    {
      Shader* shader = &material->shader;
      glUseProgram(shader->program);
      const Uniform* uniform = _findUniform(shader, name);
      glUniform1f(uniform->location, floatParam);
      checkGlError();
    }

    void setFloat(Material* material, char* name, uint32 count, float* floatParam)
    {
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

    bool setTexture(Material* material, char* name, Texture texture)
    {
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

      setInt(material, name, textureSlot); 
      return true;
    }

    void setMaterial(Renderable* renderable, Material* material)
    {
      if (renderable->material == material)
      {
        return;
      }

      renderable->material = material;
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
      context->drawCalls = (ldk::renderer::DrawCall*) LDK_GL_ALLOC(sizeof(ldk::renderer::DrawCall) * maxDrawCalls);

      if(!context->drawCalls)
      {
        LDK_GL_FREE(context);
        return nullptr;
      }

      GLuint vao;
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

      glClear(context->clearBits);

      // execute draw calls
      for (int i = 0; i < drawCallCount; i++) 
      {
        DrawCall* drawCall = context->drawCalls + i;
        _executeDrawCall(drawCall);
      }

      // reset draw call count for this frame
      context->drawCallCount = 0;
      checkGlError();
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

    Texture createTexture(const ldk::Bitmap* bitmap
        ,TextureFilter minFilter 
        ,TextureFilter magFilter
        ,TextureWrap uWrap
        ,TextureWrap vWrap)
    {
      GLuint textureId;
      glGenTextures(1, &textureId);
      glBindTexture(GL_TEXTURE_2D, textureId);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width, bitmap->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels);

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

    void destroyTexture(Texture texture)
    {
      glBindTexture(GL_TEXTURE_2D, 0);
      glDeleteTextures(1, &texture.id);
      texture.id = 0;
      texture.width = texture.height = 0;
      checkGlError();
    }

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

    static TextureWrap cfgStringToTextureWrap(const char* cfgString)
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

    static TextureFilter cfgStringToTextureFilter(const char* cfgString)
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

    static uint32 cfgStringToRenderQueue(const char* cfgString)
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

    bool loadMaterial(renderer::Material* material, const char* file)
    {

      auto cfgRoot = ldk::configParseFile(file);
      auto materialSection = ldk::configGetSection(cfgRoot, "material");

      char* fs;
      if (!ldk::configGetString(materialSection, (const char*) "frag-shader", &fs))
      {
        LogError("Error loading material. Missing Fragment shader.");
        ldk::configDispose(cfgRoot);
        return false;
      }

      char* vs;
      if(!ldk::configGetString(materialSection, (const char*) "vert-shader", &vs))
      {
        LogError("Error loading material. Missing Vertex shader.");
        ldk::configDispose(cfgRoot);
        return false;
      }


      uint32 renderQueue = (uint32) RENDER_QUEUE_OPAQUE;
      char* temp;
      if(ldk::configGetString(materialSection, (const char*) "render-queue", &temp))
      {
        renderQueue = (uint32) cfgStringToRenderQueue(temp);
        int32 queueOffset = 0;
        configGetInt(materialSection, "render-queue-offset", &queueOffset);
        renderQueue += queueOffset;
      }

      makeMaterial(material, vs, fs, renderQueue);


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
                  uWrap = cfgStringToTextureWrap(value);
                }
                else if (variant->hash == keyVWrap
                    && strncmp(STR_PARAM_V_WRAP, variant->key, keyLen) == 0)
                {
                  configGetString(section, (const char*) variant, &value);
                  vWrap = cfgStringToTextureWrap(value);
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
                  minFilter = cfgStringToTextureFilter(value);
                }
                else if (variant->hash == keyMagFilter
                    && strncmp(STR_PARAM_MAG_FILTER, variant->key, keyLen) == 0)
                {
                  configGetString(section, (const char*) variant, &value);
                  magFilter = cfgStringToTextureFilter(value);
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
                auto bmp = ldk::loadBitmap((const char*)texturePath);
                Texture texture = renderer::createTexture(bmp, minFilter, magFilter, uWrap, vWrap);
                freeAsset((void*) bmp);
                //TODO(marcio): I guess this step should be done only when binding the material...
                renderer::setTexture(material, (char*) &section->name, texture);
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
      return true;
    }

  } // renderer
} // ldk
