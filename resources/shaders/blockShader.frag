#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 SunlightDir;
in vec3 LightPos;
in vec3 Normal;

uniform bool Sunlight;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir;
    if (Sunlight) {
        lightDir = normalize(-SunlightDir);
    } else {
        lightDir = normalize(LightPos - FragPos);
    }
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * (spec * material.specular);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
