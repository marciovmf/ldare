#version 330 core
in vec2 fragCoord;
in vec4 fragColor;
uniform sampler2D texture0;
out vec4 out_color;
void main()
{
  vec4 textureColor = texture(texture0, fragCoord);
  out_color = mix(fragColor, textureColor, 0.9);
}
