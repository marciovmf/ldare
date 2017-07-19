#include <ldare\game.h>

//TODO: This is a test! Remove it when we have a renderer
#define DECLARE_EXTERN_GL
#include "../src/ldare_core_gl.h"
#undef DECLARE_EXTERN_GL

static GLint checkShaderCompilation(GLuint shader)
{
	GLint success = 0;
	GLchar infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
		std::cerr << infoLog << std::endl;
	}

	return success;
}

static GLint checkProgramLink(GLuint program)
{
	GLint success = 0;
	GLchar infoLog[512];

	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
		std::cerr << "ERROR: PROGRAM LINKING FAILED:" << infoLog << std::endl;
	}

	return success;
}

class TestGame : public ldare::Game
{
	private:
		GLuint VBO;
		GLuint VAO;
		GLuint m_ShaderProgram;
		GLuint m_VertexShader;
		GLuint m_FragmentShader;
		GLuint m_VAO;
		GLuint m_VBO;

		// Shame function nobody should copy, it is test code and should be
		// removed, deleted and burn as soon as possible
		void init_gl_stuff()
		{
					const char* vertexShaderSource = 
					"#version 330 core\n\
						layout (location = 0) in vec4 vPosition;\n\
						out vec4 vertexColor;\n\
						void main()\n\
						{\n\
							gl_Position = vPosition;\n\
						}";
									const char* fragmentShaderSource =
									"#version 330 core\n\
						out vec4 fColor;\n\
						void main()\n\
						{\n\
						    fColor = vec4(0, 0, 1, 0);\n\
						}";

			glClearColor(.6f, .7f, .8f, 0);
			const char* buff;

			// Create shader Program
			m_ShaderProgram = glCreateProgram();

			// Vertex shader
			buff = vertexShaderSource;
			m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(m_VertexShader, 1, &buff, NULL);
			glCompileShader(m_VertexShader);
			checkShaderCompilation(m_VertexShader);
			glAttachShader(m_ShaderProgram, m_VertexShader);
			glDeleteShader(m_VertexShader);

			// Fragment shader
			buff = fragmentShaderSource;
			m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(m_FragmentShader, 1, &buff, NULL);
			glCompileShader(m_FragmentShader);
			checkShaderCompilation(m_FragmentShader);
			glAttachShader(m_ShaderProgram, m_FragmentShader);
			glDeleteShader(m_FragmentShader);

			// Link shader program
			glLinkProgram(m_ShaderProgram);
			checkProgramLink(m_ShaderProgram);

			// Vertices	(X, Y, Z)
			GLfloat* vertices = new GLfloat[18] {
				-0.2f, -0.2f, 0.0f,
					0.2f, -0.2f, 0.0f,
					0.0f, 0.2f, 0.0f
			};

		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(
			GL_ARRAY_BUFFER,	
			9 * sizeof(GLfloat),
			vertices,
			GL_STATIC_DRAW);

		glVertexAttribPointer(
			0,			// Lembra do (layout = 0 ) no vertex shader ? Esse valor indica qual atributo estamos indicando
			3,			// cada vertice é composto de 3 valores
			GL_FLOAT,	// cada valor do vértice é do tipo GLfloat
			GL_FALSE,	// Quer normalizar os dados e converter tudo pra NDC ? ( no nosso caso, já esta tudo correto, então deixamos como FALSE)
			3 * sizeof(GLfloat),	// De quantos em quantos bytes, este atributo é encontrado no buffer ? No nosso caso 3 floats pros vertices + 3 floats pra cor = 6 floats
			(GLvoid*)0	// Onde está o primeiro valor deste atributo no buffer. Nesse caso, está no início do buffer
			);

		glEnableVertexAttribArray(0);			// Habilita este atributo
		glBindVertexArray(0);
		}

		void draw_stuff()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			// Use este shader
			glUseProgram(m_ShaderProgram);

			// Use este VAO e suas configurações
			glBindVertexArray(m_VAO);	
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}

	public:
		ldare::GameRuntimeSettings initialize() override
		{
			ldare::GameRuntimeSettings settings = {};
			settings.windowWidth = 800;
			settings.windowHeight = 600;

			return settings;
		}

		void startGame() override
		{
			LogInfo("Game started");
			init_gl_stuff();
		}

		void updateGame() override
		{
			LogInfo("Game Update...");
			draw_stuff();
		}
};

