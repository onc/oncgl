#version 330

in vec2 TexCoord0; 
in vec3 Normal0; 
in vec3 WorldPos0; 

layout (location = 0) out vec3 WorldPosOut; 
layout (location = 1) out vec3 DiffuseOut; 
layout (location = 2) out vec3 NormalOut; 
layout (location = 3) out vec3 TexCoordOut; 

uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform sampler2D texture_normals1;

void main() { 

    WorldPosOut = WorldPos0; 
    DiffuseOut = texture(texture_normals1, TexCoord0).xyz; 
    NormalOut = normalize(Normal0); 
    TexCoordOut = vec3(TexCoord0, 0.0); 
}
