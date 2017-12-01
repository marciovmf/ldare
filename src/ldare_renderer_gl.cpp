/**
 * win32_renderer_gl.h
 * Win32 implementation for ldare platform functions
 */

#include <ldare/ldare_asset.h>

// Sprite batch data
#define SPRITE_BATCH_MAX_SPRITES 10000
#define SPRITE_BATCH_VERTEX_DATA_SIZE sizeof(ldare::SpriteVertexData)
#define SPRITE_BATCH_SPRITE_SIZE SPRITE_BATCH_VERTEX_DATA_SIZE * 4 // 4 vetices per sprite
#define SPRITE_BATCH_BUFFER_SIZE SPRITE_BATCH_MAX_SPRITES * SPRITE_BATCH_SPRITE_SIZE
#define SPRITE_BATCH_INDICES_SIZE SPRITE_BATCH_MAX_SPRITES * 6		//6 indices per quad

#define SPRITE_ATTRIB_VERTEX 0
#define SPRITE_ATTRIB_COLOR 1
#define SPRITE_ATTRIB_UV 2


namespace ldare 
{
	static bool updateGlobalShaderData = false;
	static GlobalShaderData globalShaderData = {};

	static struct GL_SpriteBatchData
	{
		ldare::Material material;
		GLuint ubo;
		GLuint vao;
		GLuint vbo;
		GLuint ibo;
		GLvoid* gpuBuffer;
		GLvoid* gpuUniformBuffer;
		GLuint numSprites;
		uint32 spriteCount;
	} spriteBatchData;

		static int32 checkNoGlError()
	{
		const char* error = "UNKNOWN ERROR CODE";
		GLenum err (glGetError());
		int32 success = 1;
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
			LogError(error);
			err=glGetError();
		}

		return success;
	}

	static GLboolean checkShaderProgramLink(GLuint program)
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

	static GLboolean checkShaderCompilation(GLuint shader)
	{
		GLint success = 0;
		GLchar msgBuffer[1024] = {};

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

	static GLuint compileShader(const char* source, GLenum shaderType)
	{
		ASSERT(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER,
				"Invalid shader type");

		// Setup default vertex shader
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, (const GLchar**)&source, 0);
		glCompileShader(shader);
		if (!checkShaderCompilation(shader))
			return GL_FALSE;

		return shader;
	}

	static GLuint createShaderProgram(const char* vertex, const char* fragment)
	{
		GLuint vertexShader = compileShader(vertex, GL_VERTEX_SHADER);
		GLuint fragmentShader = compileShader(fragment, GL_FRAGMENT_SHADER);
		GLuint shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);	
		glAttachShader(shaderProgram, fragmentShader);

		// Link shader program
		glLinkProgram(shaderProgram);
		if (!checkShaderProgramLink(shaderProgram))
			return GL_FALSE;

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return shaderProgram;
	}

	Shader loadShader(const char* vertex, const char* fragment)
	{
		size_t vertShaderFileSize;
		size_t fragShaderFileSize;
		const char* vertexSource = (const char*)platform::loadFileToBuffer(vertex, &vertShaderFileSize);
		const char* fragmentSource = (const char*) platform::loadFileToBuffer(fragment, &fragShaderFileSize);
		Shader shader = createShaderProgram(vertexSource, fragmentSource);

		//TODO: remove this when we have a proper way to reuse file I/O memory
		platform::memoryFree((void*)vertexSource, vertShaderFileSize);			
		platform::memoryFree((void*)fragmentSource, fragShaderFileSize);			

		return shader;
	}

	int32 initSpriteBatch()
	{
		glGenVertexArrays(1, &spriteBatchData.vao);
		glGenBuffers(1, &spriteBatchData.vbo);
		glGenBuffers(1, &spriteBatchData.ibo);
		glGenBuffers(1, &spriteBatchData.ubo);
	
		// ARRAY buffer
		glBindVertexArray(spriteBatchData.vao);
		glEnableVertexAttribArray(SPRITE_ATTRIB_VERTEX);
		glEnableVertexAttribArray(SPRITE_ATTRIB_COLOR);
		glEnableVertexAttribArray(SPRITE_ATTRIB_UV);
		
		// VERTEX buffer
		glBindBuffer(GL_ARRAY_BUFFER, spriteBatchData.vbo);
		glBufferData(GL_ARRAY_BUFFER, SPRITE_BATCH_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);
		checkNoGlError();

		// Precompute indices for every sprite
		GLushort indices[SPRITE_BATCH_INDICES_SIZE]={};
		int32 offset = 0;
		for(int32 i=0; i < SPRITE_BATCH_INDICES_SIZE; i+=6)
		{
			indices[i] 	 = offset;
			indices[i+1] = offset +1;
			indices[i+2] = offset +2;
			indices[i+3] = offset +2;
			indices[i+4] = offset +3;
			indices[i+5] = offset +0;

			offset+=4; // 4 offsets per sprite
		}

		// INDEX buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteBatchData.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, SPRITE_BATCH_INDICES_SIZE * sizeof(uint16),
				&indices[0], GL_STATIC_DRAW);

		// 3 float vertex, 3 float color, 2 float uv
		glVertexAttribPointer(SPRITE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE,
				SPRITE_BATCH_VERTEX_DATA_SIZE, (const GLvoid*)0);

		glVertexAttribPointer(SPRITE_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE,
				SPRITE_BATCH_VERTEX_DATA_SIZE, (const GLvoid*)(3 * sizeof(float))); 

		glVertexAttribPointer(SPRITE_ATTRIB_UV, 2, GL_FLOAT, GL_FALSE,
				SPRITE_BATCH_VERTEX_DATA_SIZE, (const GLvoid*)(7 * sizeof(float))); 

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		// UBO buffer
		glBindBuffer(GL_UNIFORM_BUFFER, spriteBatchData.ubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalShaderData), &globalShaderData, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindVertexArray(0);
		checkNoGlError();

		//TODO: Marcio, this is a hack for testing stuff in 2D. Move this to material state
		glClearColor(1, 1, 1, 1);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEBUG_OUTPUT);
		return 1;
	}

	void begin(const ldare::Material& material)
	{
		spriteBatchData.material = material;
		spriteBatchData.spriteCount = 0;

		if ( updateGlobalShaderData)
		{
			// map UBO buffer
			glBindBuffer(GL_UNIFORM_BUFFER, spriteBatchData.ubo);
			spriteBatchData.gpuUniformBuffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
			memcpy(spriteBatchData.gpuUniformBuffer, &globalShaderData, sizeof(GlobalShaderData));
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			updateGlobalShaderData = false;
		}

		// map VERTEX buffer
		glBindBuffer(GL_ARRAY_BUFFER, spriteBatchData.vbo);
		spriteBatchData.gpuBuffer = (GLvoid*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	}

	void submit(const Sprite& sprite)
	{
		Material& material = spriteBatchData.material;
		// sprite vertex order 0,1,2,2,3,0
		// 1 -- 2
		// |    |
		// 0 -- 3
		// bind uniform
		unsigned int block_index = glGetUniformBlockIndex(material.shader, "ldare");
		const GLuint bindingPointIndex = 0;
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointIndex, spriteBatchData.ubo);

		// map pixel coord to texture space
		Rectangle uvRect = sprite.srcRect;
		uvRect.x = uvRect.x / material.texture.width;
		uvRect.y = (uvRect.y / material.texture.height);
		uvRect.w = uvRect.w / material.texture.width;
		uvRect.h =  (uvRect.h / material.texture.height);

		spriteBatchData.spriteCount++;
		SpriteVertexData* vertexData = (SpriteVertexData*) spriteBatchData.gpuBuffer;

		// top left
		vertexData->color = sprite.color;
		vertexData->position = Vec3{sprite.position.x, sprite.position.y + sprite.height, sprite.position.z};
		vertexData->uv = { uvRect.x, uvRect.y + uvRect.h};
		vertexData++;

		// bottom left
		vertexData->color = sprite.color;
		vertexData->position = sprite.position;
		vertexData->uv = { uvRect.x, uvRect.y};
		vertexData++;

		// bottom right
		vertexData->color = sprite.color;
		vertexData->position = 
			Vec3 {sprite.position.x + sprite.width,	sprite.position.y, sprite.position.z};
		vertexData->uv = { uvRect.x + uvRect.w, uvRect.y};
		vertexData++;

		// top right
		vertexData->color = sprite.color;
		vertexData->position = 
			Vec3 {sprite.position.x + sprite.width, sprite.position.y + sprite.height,sprite.position.z};
		vertexData->uv = {uvRect.x + uvRect.w, uvRect.y + uvRect.h};
		vertexData++;

		spriteBatchData.gpuBuffer = (void*) vertexData;
	}

	void flush()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void end()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindTexture(GL_TEXTURE_2D, spriteBatchData.material.texture.id);
		glUseProgram(spriteBatchData.material.shader);
		glBindVertexArray(spriteBatchData.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteBatchData.ibo);
		glDrawElements(GL_TRIANGLES, 6 * spriteBatchData.spriteCount, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//TODO: sort draw calls per material 
	}

	ldare::Texture loadTexture(const char* bitmapFile)
	{
		ldare::Bitmap bitmap;
		ldare::loadBitmap(bitmapFile, &bitmap);
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.height, 0, 
				GL_RGBA, GL_UNSIGNED_BYTE, bitmap.pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		//TODO: make filtering paremetrizable when importing texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
		ldare::freeAsset(bitmap.bmpFileMemoryToRelease_, bitmap.bmpMemorySize_);

		ldare::Texture texture = {};
		texture.width = bitmap.width;
		texture.height = bitmap.height;
		texture.id = textureId;
		return texture;
	}

	void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight, uint32 virtualWidth, uint32 virtualHeight)
	{
		float targetAspectRatio = virtualWidth / (float) virtualHeight;
		// Try full viewport width with cropped, height if necessary
		int32 viewportWidth = windowWidth;
		int32 viewportHeight = (int)(viewportWidth / targetAspectRatio + 0.5f);

		// if calculated viewport height does not fit the window width,
		// switch to pillar box to preserve the aspect ratio
		if (viewportHeight > windowHeight)
		{
			viewportHeight = windowHeight;
			viewportWidth = (int)(viewportHeight* targetAspectRatio + 0.5f);
		}

		// center viewport
		int32 viewportX = (windowWidth / 2) - (viewportWidth / 2);
		int32 viewportY = (windowHeight / 2) - (viewportHeight / 2);
		setViewport(viewportX, viewportY, viewportWidth, viewportHeight);

		globalShaderData.baseModelMatrix = Mat4();
		globalShaderData.baseModelMatrix.scale(
				viewportWidth / (float)virtualWidth, 
				viewportHeight / (float)virtualHeight, 1.0f);
	}
 
	void setViewport(uint32 x, uint32 y, uint32 width, uint32 height)
	{
		globalShaderData.projectionMatrix.orthographic(0, width, 0, height, -1, 1);
		glViewport(x, y, width, height);
		updateGlobalShaderData = true;
	}

	void updateRenderer(float deltaTime)
	{
		globalShaderData.time.x = deltaTime;
		globalShaderData.time.y += deltaTime;
		updateGlobalShaderData = true;
	}
} // namespace ldare
