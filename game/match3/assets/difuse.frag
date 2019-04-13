#version 330 core
in vec2 fragCoord;
uniform sampler2D texture0;
out vec4 out_color;
void main()
{
  vec4 solidColor = vec4(1.0, 1.0, 1.0, 1.0); 
  vec4 textureColor = texture(texture0, fragCoord);
  out_color = mix(solidColor, textureColor, 0.9);
}
