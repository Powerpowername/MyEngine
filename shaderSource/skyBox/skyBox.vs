#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main()
{
    TexCoords = aPos;//这边传的是立方体的局域坐标系，所以我外面怎么变model都不会影响到纹理的采样坐标（任然是从(0,0,0)开始的方向向量）
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    // gl_Position = pos.xyww;
    //这样的天空盒子才满足我此处的需求
    gl_Position = pos;
}  