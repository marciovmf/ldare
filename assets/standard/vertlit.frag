 #version 330 core

in vec4 vertexColor;
in vec2 fragCoord;
out vec4 out_color;
uniform sampler2D texture0;

// light test
in vec3 lightColor;
in vec3 fragPos;
in vec3 lightPos;
in vec3 normal;

void main()
{
  float ambientIntensity = 0.08;
  vec3 ambientLight = vec3(0, 1, 1);
	
	// Ambient light
	vec3 ambientComponent = (ambientLight * ambientIntensity);

	// Diffuse light
	vec3 norm = normalize(normal);
	vec3 lightDir =  normalize(lightPos - fragPos);
	float angle =  max(dot(norm, lightDir),0.0);
	vec3 diffuseComponent = angle * lightColor;
	
	vec3 result = (ambientComponent + diffuseComponent);
	out_color = vec4(result, 1.0) * texture(texture0, fragCoord);
	
}
