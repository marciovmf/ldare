#version 330 core
out vec4 color;
in vec4 fragColor;
in vec2 texCoord;
layout (std140) uniform ldare_t
{ 
	mat4 projectionMatrix;
	mat4 baseModelMatrix;
	vec2 time;
} ldare;

uniform sampler2D mainTexture;

vec2 coord = vec2( texCoord.x, texCoord.y);

void main() 
{
	color = texture(mainTexture, coord);
	//TODO: this is a hack! Remove this when we properly order our drawcalls!
	if (color.xyz==vec3(0)) discard;

	color = vec4(1.0, 1.0, 1.0, 0.0);
}

