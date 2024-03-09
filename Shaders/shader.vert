#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec4 fragPosLightSpace;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightProjection;
uniform mat4 lightView;

void main()
{
    vec3 FragPos = vec3(model * vec4(pos, 1.0));
    fragPosLightSpace = lightProjection * lightView * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoord = aTexCoord;
    Normal = aNormal;
}