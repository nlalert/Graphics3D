#version 330 core

in vec3 FragPos;        // Fragment position in world space
in vec3 Normal;         // Normal in world space
in vec2 TexCoord;       // Texture coordinates

out vec4 FragColor;     // Output fragment color

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLight;
uniform vec3 viewPos;
uniform sampler2D texture1;

void main()
{
    // Ambient lighting
    vec3 ambient = pointLight.ambient * texture(texture1, TexCoord).rgb;

    // Diffuse lighting
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * (diff * texture(texture1, TexCoord).rgb);

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = pointLight.specular * (spec * texture(texture1, TexCoord).rgb);

    // Attenuation
    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    // Final light intensity
    vec3 lightColor = ambient + (diffuse + specular) * attenuation;

    FragColor = vec4(lightColor, 1.0);
}