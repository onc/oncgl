#version 330 

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord0; 
out vec3 Normal0; 
out vec3 WorldPos0; 

void main() { 

    gl_Position = projection * view * model * vec4(position, 1.0);
    TexCoord0 = texCoords; 
    Normal0 = (model * vec4(normal, 0.0)).xyz;
    WorldPos0 = (model * vec4(position, 1.0)).xyz;
}
