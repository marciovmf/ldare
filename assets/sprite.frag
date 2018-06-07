#version 330 core
out vec4 color;
in vec4 fragColor;
in vec2 texCoord;

layout (std140) uniform ldk
{ 
	mat4 projectionMatrix;
	mat4 baseModelMatrix;
	vec4 time;
};

uniform sampler2D mainTexture;
vec2 coord = vec2(texCoord.x, texCoord.y);

void main() 
{
	color = texture(mainTexture, coord) * fragColor;
}
