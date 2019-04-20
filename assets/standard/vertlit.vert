#version 330 core

in vec3 _pos;
in vec3 _normal;
in vec2 _uv;

uniform mat4 mmodel;
uniform mat4 mprojection;

out vec2 fragCoord;

// light test
out vec3 lightColor;
out vec3 fragPos;
out vec3 lightPos;
out vec3 normal;

void main()
{
	gl_Position = mprojection * mmodel * vec4(_pos,1.0);
	fragCoord = _uv;
	normal = mat3(transpose(inverse(mmodel))) * _normal;
	
	// light test
	lightColor = vec3(.8, .8, .8);
	lightPos = vec3(500, 0, 300);
	fragPos =  vec3(mmodel * vec4(_pos,1.0));
}
