/**
 * win32_renderer_gl.h
 * Win32 implementation for ldare platform functions
 */

// Sprite batch data
#define SPRITE_BATCH_MAX_SPRITES 10000
#define SPRITE_BATCH_VERTEX_DATA_SIZE sizeof(ldare::render::SpriteVertexData)
#define SPRITE_BATCH_SPRITE_SIZE SPRITE_BATCH_VERTEX_DATA_SIZE * 4 // 4 vetices per sprite
#define SPRITE_BATCH_BUFFER_SIZE SPRITE_BATCH_MAX_SPRITES * SPRITE_BATCH_SPRITE_SIZE
#define SPRITE_BATCH_INDICES_SIZE SPRITE_BATCH_MAX_SPRITES * 6		//6 indices per quad

#define SPRITE_ATTRIB_VERTEX 0
#define SPRITE_ATTRIB_COLOR 1

namespace ldare 
{
	namespace render
	{
		static struct GL_SpriteBatchData
		{
			GLuint defaultShader;
			GLuint vao;
			GLuint vbo;
			GLuint ibo;
			GLvoid* gpuBuffer;
			GLuint numSprites;
			uint32 spriteCount;
		} spriteBatchData;

	//	static void resetGlError()
	//	{
	//		// reset opengl error flag
	//		GLenum err;
	//		do {
	//			err = glGetError();
	//		}
	//		while(err!= GL_NO_ERROR);
	//	}

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
				else if ( shaderType == GL_VERTEX_SHADER)
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
			//TODO: remove hardcoded shader source and load it from argument path
			const char* vertexSource = "#version 330 core\n\
	layout (location = 0) in vec3 vPos;\n\
	layout (location = 1) in vec3 vColor;\n\
	out vec4 fragColor;\n\
	void main(){ fragColor = vec4(vColor, 0.0);\n\
		gl_Position = vec4(vPos, 1.0);}\n\0";

			const char* fragmentSource = "#version 330 core\n\
out vec4 color;\n\
in vec4 fragColor;\n\
void main() {	color = fragColor; }\n\0";

			Shader shader = createShaderProgram(vertexSource, fragmentSource);
			return shader;
		}

		int32 initSpriteBatch()
		{
			glGenVertexArrays(1, &spriteBatchData.vao);
			glGenBuffers(1, &spriteBatchData.vbo);
			glGenBuffers(1, &spriteBatchData.ibo);

			glBindVertexArray(spriteBatchData.vao);
			glEnableVertexAttribArray(SPRITE_ATTRIB_VERTEX);
			glEnableVertexAttribArray(SPRITE_ATTRIB_COLOR);

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
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteBatchData.ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, SPRITE_BATCH_INDICES_SIZE * sizeof(uint16),
					&indices[0], GL_STATIC_DRAW);

			// 3 float vertex, 3 float color
			glVertexAttribPointer(SPRITE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE,
					SPRITE_BATCH_VERTEX_DATA_SIZE, (const GLvoid*)0);

			glVertexAttribPointer(SPRITE_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE,
					SPRITE_BATCH_VERTEX_DATA_SIZE, (const GLvoid*)(3 * sizeof(float))); 


			//TODO: Add texture, uv1, uv2, normal(?), tangent(?)
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			checkNoGlError();

			glDisable(GL_CULL_FACE);
			return 1;
		}

		void begin()
		{
			spriteBatchData.spriteCount = 0;
			glBindBuffer(GL_ARRAY_BUFFER, spriteBatchData.vbo);
			checkNoGlError();
			spriteBatchData.gpuBuffer = (GLvoid*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			checkNoGlError();
		}

		void submit( uint32 shader, const Sprite& sprite)
		{
			// sprite vertex order 0,1,2,2,3,0
			// 0 -- 3
			// |    |
			// 1 -- 2

			spriteBatchData.defaultShader = shader;
			spriteBatchData.spriteCount++;
			//TODO: store draw calls somewher to be able to sort them per material
			//later
			SpriteVertexData* vertexData = (SpriteVertexData*) spriteBatchData.gpuBuffer;
			vertexData->position = sprite.position;
			vertexData->color = sprite.color;
			vertexData++;
			
			// bottom left
			vertexData->position = Vec3 {
				sprite.position.x,
					sprite.position.y + sprite.height,
					sprite.position.z};
			vertexData->color = sprite.color;
			vertexData++;

			// bottom right
			vertexData->position = Vec3 {
				sprite.position.x + sprite.width,
					sprite.position.y + sprite.height,
					sprite.position.z};
			vertexData->color = sprite.color;
			vertexData++;

			// top right
			vertexData->position = Vec3 {
				sprite.position.x + sprite.width,
				sprite.position.y,
				sprite.position.z};
			vertexData->color = sprite.color;
			vertexData++;
			spriteBatchData.gpuBuffer = (void*) vertexData;
		}

		void end()
		{
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glClearColor(.3, .3, .3, 0);
			//TODO: sort draw calls per material 
		}
		
		void flush()
		{
			glUseProgram(spriteBatchData.defaultShader);
			checkNoGlError();
			glBindVertexArray(spriteBatchData.vao);
			checkNoGlError();
			glClear(GL_COLOR_BUFFER_BIT);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteBatchData.ibo);
			checkNoGlError();
			glDrawElements(GL_TRIANGLES, 6 * spriteBatchData.spriteCount, GL_UNSIGNED_SHORT, 0);
			checkNoGlError();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

	} // namespace renderer
} // namespace ldare
