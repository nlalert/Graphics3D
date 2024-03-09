//Fragment Shader
#version 330

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 fragPosLightSpace;

out vec4 colour;

uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture2D;
uniform sampler2D shadowMap;

float ShadowCalculation()
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float bias = 0.005;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}
vec3 ambientLight()
{
    float ambientStrength = 0.3f;
    vec3 ambient = lightColour * ambientStrength;
    return ambient;
}

vec3 diffuseLight()
{
    float diffuseStrength = 0.5f;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = lightColour * diff * diffuseStrength;
    return diffuse;
}

vec3 specularLight()
{
    float specularStrength = 0.8f;
    float shininess = 256.0f;//= 64 for phong
     
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 reflectDir = reflect(-lightDir, norm);

    //Phong Shading
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

    //Blinn-Phong Shading
    vec3 halfDir = (viewDir + lightDir)/2.0f;
    float spec = pow(max(dot(halfDir, norm), 0.0f), shininess);
    vec3 specular = lightColour * spec * specularStrength;
    return specular;
}

void main()
{
    float shadow = ShadowCalculation();
    colour = texture(texture2D, TexCoord) * vec4((1.0 - shadow)*(ambientLight() + diffuseLight() + specularLight()), 1.0f);
}