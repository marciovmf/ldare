#version 330 core
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec3 vPos;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in float vZRotation;

layout (std140) uniform ldare_t
{ 
	mat4 projectionMatrix;
	mat4 baseModelMatrix;
	vec2 time;
} ldare;

out vec4 fragColor;
out vec2 texCoord;
void main()
{
	fragColor = vColor;
	gl_Position = ldare.projectionMatrix * ldare.baseModelMatrix * vec4(vPos.xyz, 1.0);
	texCoord = vTexCoord ;
}

