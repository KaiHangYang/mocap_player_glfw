#version 330 core

out vec3 color;
in float normalWeight;
in float specWeight;
in vec3 fColor;

void main() {
	float ambientStrength = 0.15;
	float specularStrength = 0.4;

	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 elmColor = fColor;
	
	vec3 ambient = ambientStrength * lightColor;
	vec3 diffuse = normalWeight * lightColor;
	vec3 specular = specWeight * specularStrength * lightColor;

	color = (diffuse + ambient + specular) * elmColor;
	
    //color = vec3(0.0, 0.0, 1.0);
}
