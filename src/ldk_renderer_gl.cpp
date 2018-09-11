
namespace ldk
{
  // Internal functions
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

  VertexAttributeType _glTypeToInternal(uint32 glType)
  {
    switch (glType)
    {
      case GL_INT:
      case GL_INT_VEC2:
      case GL_INT_VEC3:
      case GL_INT_VEC4:
        return VertexAttributeType::ATTRIB_TYPE_INT;

      case GL_FLOAT:
      case GL_FLOAT_VEC2:
      case GL_FLOAT_VEC3:
      case GL_FLOAT_VEC4:
      case GL_FLOAT_MAT2:
      case GL_FLOAT_MAT3:
      case GL_FLOAT_MAT4:
        return VertexAttributeType::ATTRIB_TYPE_FLOAT;

      case GL_BOOL:
      case GL_BOOL_VEC2:
      case GL_BOOL_VEC3:
      case GL_BOOL_VEC4:
        return VertexAttributeType::ATTRIB_TYPE_BOOL;

      case GL_SAMPLER_2D:
      case GL_SAMPLER_3D:
        return VertexAttributeType::ATTRIB_TYPE_SAMPLER;

      default:
        return VertexAttributeType::ATTRIB_TYPE_UNKNOWN;
    }

  }

  // Shader functions
  int32 loadShader(Shader* shader, char* vertexSource, char* fragmentSource)
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

    LDK_ASSERT(uniformCount <= LDK_MAX_UNIFORM_COUNT, "Too many uniforms");

    uint32 maxUniformNameLength = sizeof(char) * LDK_UNIFORM_NAME_LENGTH;

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
    return shader->program > 0;
  }

  RenderContext* makeRenderContext(uint32 maxDrawCalls, uint32 clearBits, uint32 settingsBits)
  {
    RenderContext* context = (RenderContext*) platform::memoryAlloc(sizeof(RenderContext));
    if (!context) return nullptr;

    context->clearBits = clearBits;
    context->settingsBits = settingsBits;
    context->maxDrawCalls = maxDrawCalls;
    context->drawCallCount = 0;
    context->drawCalls = (ldk::DrawCall*) platform::memoryAlloc(sizeof(ldk::DrawCall) * maxDrawCalls);
        
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

  // Context functions
  void freeContext(RenderContext* context)
  {
    platform::memoryFree(context->drawCalls); 
    platform::memoryFree(context); 
  }

  // Data functions
  void setVertexData(VertexData* data, uint32 bufferSize, uint32 stride, bool isStatic) 
  {
    data->size = bufferSize;
    data->stride = stride;
    data->usage = isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
    data->attributCount = 0;
  }

  void makeRenderable(Renderable* renderable, VertexData* vertexData)
  {
    renderable = {};
    renderable->data = *vertexData;
    //TODO(marcio): use triple buffer when usage is NOT STATIC
    renderable->bufferCount = 1;
    renderable->needUpdate = 1;
  }

  void setShader(Renderable* renderable, Shader* shader)
  {
    renderable->shader = shader;
    glGetProgramiv(shader->program, GL_ACTIVE_ATTRIBUTES, (GLint*) renderable->attributeCount);
    
    if(renderable->attributeCount != renderable->data.attributCount)
    {
      LogWarning("Shader and vertex data has different attribute count");
    }

    uint32 attribType;
    uint32 attribSize = 0;
    uint64 attribHash = 0;
    char attribName[256];
   
    int32 attribCount = renderable->data.attributCount;
    for (int i = 0; i < attribCount; i++) 
    {
      // query attribute type and size
      glGetActiveAttrib(shader->program, i, 256, 0, (GLint*) attribSize,  (GLenum*) &attribType, attribName);
      attribType = _glTypeToInternal(attribType);
      VertexAttribute* attribute;

      for (int j = 0; j < attribCount; j++) 
      {
        VertexAttribute* tmpAttribute = renderable->data.attributes + j;
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

    GLuint usage = renderable->data.usage;

    for (int i = 0; i < renderable->bufferCount; i++) 
    {
      GLuint* vbo = renderable->buffers + i;
      
      glGenBuffers(1, vbo);
      glBindBuffer(GL_ARRAY_BUFFER, *vbo);
      glBufferData(GL_ARRAY_BUFFER, renderable->data.size + renderable->data.stride, NULL, usage);
      renderable->fences[i] = (size_t) glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
} // ldk
