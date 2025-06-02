#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcood;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;//副切线，用来计算TBN矩阵，法线贴图使用时会需要他

uniform mat4 uniform mat4 modelMat; 
uniform mat4 viewMat;
uniform mat4 projMat;


/*
顶点着色器一般输出
顶点数据，纹理坐标
*/

//out vec4 vertexColor;
out vec2 TexCoord;
out vec3 fragPos;//世界坐标系下模型的坐标，方便后续光照计算的
out vec3 Normal;
out mat3 tbn;

void mian()
{
	gl_Position = projMat*viewMat*modelMat*vec4(aPos,1.0f);//*transform;
    fragPos = (modelMat*vec4(aPos.xyz,1.0f)).xyz;

    Normal=mat3(transpose(inverse(modelMat)))*aNormal;//防止法向量变形
    vec3 T = normalize(vec3(modelMat * vec4(tangent,   0.0)));
	vec3 B = normalize(vec3(modelMat * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(modelMat * vec4(aNormal,    0.0)));
    tbn = mat3(T, B, N);
}