#version 330 core
in vec4 _color;
in vec3 _pos; 
in vec2 _uv;

out vec4 fragColor;
out vec2 fragCoord;
uniform mat4 mmodel;
uniform mat4 mprojection;
void main()
{
  gl_Position = mprojection * mmodel * vec4(_pos, 1.0);
  fragColor = _color;
  fragCoord = _uv;
}
