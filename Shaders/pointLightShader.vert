// Vertex shader for point light
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 FragPos;       // Fragment position in world space
out vec3 Normal;        // Normal in world space
out vec2 TexCoord;      // Texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal; // Transform normal to world space
    TexCoord = texCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}