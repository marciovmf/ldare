#version 330 core
in vec3 _pos; 
in vec2 _uv;

out vec4 vertexColor;
out vec2 fragCoord;
uniform mat4 mmodel;
uniform mat4 mprojection;
void main()
{
  gl_Position = mprojection * mmodel * vec4(_pos, 1.0);
  vertexColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  fragCoord = _uv;
}
