#version 330 core
in vec4 fragColor;
in vec2 fragCoord;
uniform sampler2D texture0;
out vec4 out_color;
void main()
{
  vec4 textureColor = texture(texture0, fragCoord);
  out_color = mix(fragColor, textureColor, 0.2);
}
