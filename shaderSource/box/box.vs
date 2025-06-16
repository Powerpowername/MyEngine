#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
// layout (location = 4) in vec3 bitangent;

out VS_OUT {
    vec2 TexCoords;
    // vec3 LightPos;//不需要在这里知道
    vec3 ViewPos;
    vec3 FragPos;
    mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 viewPos;

void main()
{
    gl_Position = projection * view * model * vec4(position,1.0f);

    vs_out.FragPos = vec3(model * vec4(position, 1.0));   
    vs_out.TexCoords = texCoords;
    mat3 normalMatrix = transpose(inverse(mat3(model)));//避免向量在转换到世界坐标系时变形
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);    
    vec3 B = normalize(normalMatrix * cross(normal,tangent));
    
    // vs_out.TBN = transpose(mat3(T, B, N));  
    vs_out.TBN = mat3(T, B, N);  
    vs_out.ViewPos = viewPos;
}