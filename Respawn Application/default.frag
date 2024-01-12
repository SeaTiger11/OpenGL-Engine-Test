#version 330 core
out vec4 FragColour;

//Inputs the current position from the vertex shader
in vec3 crntPos;
//Inputs the normal from the vertex shader
in vec3 Normal;
//Inputs colour from the vertex shader
in vec3 colour;
//Inputs texture coordinate from the vertex shader
in vec2 texCord;

//Uniform texture variables
uniform sampler2D diffuse0;
uniform sampler2D specular0;

//Uniform cam position
uniform vec3 camPos;

struct PointLight {
	vec4 lightColour;
	vec3 lightPos;
	float a;
	float b;
	float c;
};

struct DirectionalLight {
	vec4 lightColour;
	vec3 lightDirection;
};

struct SpotLight {
	vec4 lightColour;
	vec3 lightPos;
	vec3 lightDirection;
	float innerCone;
	float outerCone;
};

const int MAX_POINT_LIGHTS = 2;
const int MAX_DIRECTIONAL_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform int pointLightCount = 0;
uniform int directionalLightCount = 0;
uniform int spotLightCount = 0;

vec4 Point(PointLight pointLight) {
	//The light attenuation (light drop off)
	vec3 lightVec = pointLight.lightPos - crntPos;
	float dist = length(lightVec);
	float inten = 1.0f / (pointLight.a * dist * dist + pointLight.b * dist + pointLight.c);

	//The diffused lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);
	
	//The specular lighting
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCord) * diffuse * inten + texture(specular0, texCord).r * specular * inten) * pointLight.lightColour;
}

vec4 Directional(DirectionalLight directionalLight) {
	//The diffused lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(-directionalLight.lightDirection);
	float diffuse = max(dot(normal, lightDirection), 0.0f);
	
	//The specular lighting
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCord) * diffuse + texture(specular0, texCord).r * specular) * directionalLight.lightColour;
}

vec4 Spot(SpotLight spotLight) {
	//The diffused lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(spotLight.lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);
	
	//The specular lighting
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	//The light attenuation (light drop off)
	float angle = dot(spotLight.lightDirection, -lightDirection);
	float inten = clamp((angle - spotLight.outerCone) / (spotLight.innerCone - spotLight.outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texCord) * diffuse * inten + texture(specular0, texCord).r * specular * inten) * spotLight.lightColour;
} 


void main() {
	//Ambient lighting
	float ambient = 0.2f;
	vec4 totalColour = texture(diffuse0, texCord) * ambient;

	//Adds the lighting contributions from all the lights in the scene
	for (int i = 0; i < pointLightCount; i++) {
		totalColour += Point(pointLights[i]);
	}
	for (int i = 0; i < directionalLightCount; i++) {
		totalColour += Directional(directionalLights[i]);
	}
	for (int i = 0; i < spotLightCount; i++) {
		totalColour += Spot(spotLights[i]);
	}

	//Sets the output FragColour to the total colour
	FragColour = totalColour;
}