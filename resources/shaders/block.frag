#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
flat in int vTexIndex;

uniform bool DoSunlight;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2DArray texture_diffuse1;
    vec3 specular;
    float shininess;
};

// #define NR_POINT_LIGHTS 2
// uniform PointLight PointLights[NR_POINT_LIGHTS];
uniform DirLight Sunlight;
uniform Material material;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos);
    vec3 result = vec3(0.0);

    result += calcDirLight(Sunlight, norm, viewDir);
    // for (int i = 0; i < NR_POINT_LIGHTS; i++) {
    //     result += calcPointLight(PointLights[i], norm, FragPos, viewDir);
    // }

    FragColor = vec4(result, 1.0);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, vec3(TexCoord, vTexIndex)));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, vec3(TexCoord, vTexIndex)));
    vec3 specular = light.specular * (spec * material.specular);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, vec3(TexCoord, vTexIndex)));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, vec3(TexCoord, vTexIndex)));
    vec3 specular = light.specular * (spec * material.specular);
    return (ambient + diffuse + specular);
}
