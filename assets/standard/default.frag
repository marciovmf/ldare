#version 330 core
in vec4 vertexColor;
in vec2 fragCoord;
out vec4 out_color;
uniform sampler2D texture0;
void main()
{
  vec4 color = texture2D(texture0, fragCoord);
  out_color = vec4(color.rgb, 1.0);
}
