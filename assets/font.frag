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
	 vec4 textureColor = texture(mainTexture, coord);
	 //color = vec4(fragColor.rgb, textureColor.a);

	 color = vec4(fragColor.rgb, fragColor.a * textureColor.a);
	 //color = (fragColor.rgba) * textureColor.a;
}

