#version 330 core
layout (location = 0) in vec4 vColor;
layout (location = 1) in vec3 vPos;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in float vZRotation;

layout (std140) uniform ldk
{ 
	mat4 projectionMatrix;
	mat4 baseModelMatrix;
	vec4 time;
};

out vec4 fragColor;
out vec2 texCoord;
void main()
{
	fragColor = vColor;
	//gl_Position = projectionMatrix * baseModelMatrix * vec4(vPos.xyz, 1.0);
	gl_Position = projectionMatrix * vec4(vPos.xyz, 1.0);
	texCoord = vTexCoord;
}
