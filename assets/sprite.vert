#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexCoord;
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
	fragColor = vec4(vColor, 0.0);
	gl_Position = ldare.projectionMatrix * ldare.baseModelMatrix * vec4(vPos.xy, 1.0, 1.0);
	texCoord = vTexCoord;
}
