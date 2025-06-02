#version 330 core
#define NR_POINT_LIGHTS 4 //光源数量
//后期考虑一下光源在哪里画
//光照模型采用Bling-Phong 模型
struct Material
{
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    sampler2D texture_normal0;
    vec3 color;//?
    float shininess;
};

struct LightDirectional//平行光
{
    bool flag;
	vec3 pos;
	vec3 color;
	vec3 dirToLight;//指向光源方向向量
};

struct LightSpot//聚光(手电筒)
{
	bool flag;
	vec3 pos;
	vec3 color;
	vec3 dirToLight;

    //衰减参数
	float constant;
	float linear;
	float quadratic;

    //聚光范围
	float cosPhyInner;//内圈
	float cosPhyOuter;//外圈
};

struct LightPoint//点光源
{
	bool flag; 
	vec3 pos;
	vec3 color;
	vec3 dirToLight;

    //聚光范围
	float constant;
	float linear;
	float quadratic;
};

in vec3 fragPos;
in vec3 Normal;
in vec2 TexCoord;
in mat3 tbn;

uniform Material material;
uniform LightDirectional lightDirectional[NR_POINT_LIGHTS];
uniform LightPoint lightPoint[NR_POINT_LIGHTS];
uniform LightSpot lightSpot[NR_POINT_LIGHTS];

uniform vec3 cameraPos;

out vec4 FragColor;

vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera);
vec3 CalcLightPoint(LightPoint light,vec3 uNormal,vec3 dirToCamera);
vec3 CalcLightSpot(LightSpot light,vec3 uNormal,vec3 dirToCamera);

void main()
{

}
//漫反射不需要考虑半程向量，镜面光才需要，环境光也不需要
vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera)
{
    //漫反射
    float diffIntensity = max(0,dot(light.dirToLight,uNormal));//漫反射强度
    vec3 diffColor = diffIntensity * light.color * texture(material.texture_diffuse0,TexCoord).rgb;
    
    //镜面反射
    //vec3 R=normalize(reflect(-light.dirToLight,uNormal));//Pong式光照模型采用反射向量
    
    vec3 halfwayDir = light.dirToLight 
    vec3 specIntensity = 

}