#version 330 core
#define NR_POINT_LIGHTS 4 //光源数量
struct Material
{
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    sampler2D texture_normal0;//normal本身就是切线空间内的坐标
    float shininess;
};

struct LightDirectional//平行光
{
    bool flag;
    vec3 color;
	vec3 pos;
	vec3 dirToLight;//指向光源方向向量
};

// struct LightSpot//聚光(手电筒)
// {
// 	bool flag;
// 	vec3 pos;
// 	vec3 dirToLight;
//     vec3 color;
//     //衰减参数
// 	float constant;
// 	float linear;
// 	float quadratic;

//     //聚光范围
// 	float cosPhyInner;//内圈
// 	float cosPhyOuter;//外圈
// };

// struct LightPoint//点光源
// {
// 	bool flag; 
// 	vec3 pos;
// 	vec3 dirToLight;
//     vec3 color;
//     //聚光范围
// 	float constant;
// 	float linear;
// 	float quadratic;
// };

uniform Material material;
uniform LightDirectional lightDirectional[NR_POINT_LIGHTS];
// uniform LightPoint lightPoint[NR_POINT_LIGHTS];
// uniform LightSpot lightSpot[NR_POINT_LIGHTS];

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    // vec3 LightPos;//不需要在这里知道
    vec3 ViewPos;
    vec3 FragPos;
    mat3 TBN;
} fs_in;

//后期考虑一下光源在哪里画
//光照模型采用Bling-Phong 模型
//采用在切线空间中计算光照，像素着色器（片段着色器）的开销一般大于顶点着色器，所以将尽可能多的操作交给顶点着色器


vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera);
// vec3 CalcLightPoint(LightPoint light,vec3 uNormal,vec3 dirToCamera);
// vec3 CalcLightSpot(LightSpot light,vec3 uNormal,vec3 dirToCamera);

void main()
{
    vec3 color = vec3(0,0,0);
    vec3 uNormal = normalize(texture(material.texture_normal0,fs_in.TexCoords).rgb*2.0f-1.0f);
	uNormal = normalize(fs_in.TBN * uNormal);
    /*
    考虑一下法线向量是顶点数据传入的，还是使用纹理传入的，此处先采用纹理传入的处理一下
    */
    //法线数据是[-1,1]之间，存储时使用下面操作转换到[0,1]
    //vec3 rgb_normal = normal * 0.5 + 0.5; // 从 [-1,1] 转换至 [0,1]
    // vec3 uNormal = normalize(texture(material.texture_normal0,TexCoords).rgb * 2.0f - 1.0f);//使用时转换回[-1,1]

    vec3 dirToCamera = normalize(fs_in.ViewPos - fs_in.FragPos);
    for(int i = 0;i < NR_POINT_LIGHTS;i++)
	{
		if(lightDirectional[i].flag)
			color+=CalcLightDirectional(lightDirectional[i],uNormal,dirToCamera);
		
		// if(lightPoint[i].flag)
		// 	color+=CalcLightPoint(lightPoint[i],uNormal,dirToCamera);
		
		// if(lightSpot[i].flag)
		// 	color+=CalcLightSpot(lightSpot[i],uNormal,dirToCamera);
	}
    FragColor=vec4(color,1.0f);
    // FragColor = texture(material.texture_diffuse0, fs_in.TexCoords);

}
//漫反射不需要考虑半程向量，镜面光才需要，环境光也不需要
vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera)
{
    //漫反射
    float diffIntensity = max(0,dot(light.dirToLight ,uNormal));//漫反射强度
    vec3 diffColor = diffIntensity * light.color * texture(material.texture_diffuse0,fs_in.TexCoords).rgb;
    
    //镜面反射
    //vec3 R=normalize(reflect(-light.dirToLight,uNormal));//Pong式光照模型采用反射向量
    //使用半程向量计算镜面光
    vec3 halfwayDir = normalize(light.dirToLight + dirToCamera);
    float specIntensity = pow(max(dot(uNormal,halfwayDir),0.0),material.shininess);
    vec3 specularColor = specIntensity * light.color * texture(material.texture_specular0,fs_in.TexCoords).rgb;
    return diffColor + specularColor;
}

// //有衰减的点光源
// vec3 CalcLightPoint(LightPoint light,vec3 uNormal,vec3 dirToCamera)
// {
//     float dist = length(light.pos - fs_in.FragPos);
//     float attenuation = 1.0f/(light.constant + light.linear * dist + light.quadratic*(dist * dist));//衰减系数

//     //diffuse
//     float diffIntensity = max(0,dot(light.pos - fs_in.FragPos,uNormal)) * attenuation;
//     vec3 diffColor = diffIntensity * light.color * texture(material.texture_diffuse0 ,fs_in.TexCoords).rgb;

//     //specular
//     vec3 halfwayDir = normalize(light.dirToLight + dirToCamera);
//     float specIntensity = pow(max(0,dot(uNormal,halfwayDir)),material.shininess);
//     vec3 specularColor = specIntensity * light.color * texture(material.texture_specular0,fs_in.TexCoords).rgb * attenuation;


//     return diffColor + specularColor;
// }

// //柔和聚光
// vec3 CalcLightSpot(LightSpot light,vec3 uNormal,vec3 dirToCamera)
// {
//     float dist = length(light.pos -fs_in.FragPos);
//     float attenuation = 1.0f/(light.constant + light.linear * dist + light.quadratic * (dist * dist));

//     float theta = dot(normalize(fs_in.FragPos - light.pos),-light.dirToLight);
//     float epsilon = (light.cosPhyInner - light.cosPhyOuter);
//     float intensity = clamp((theta - light.cosPhyOuter) / epsilon, 0.0, 1.0);//clamp是钳位函数或限幅函数，将值限定在0-1之间

//     //diffuse
//     float diffIntensity = max(dot(light.dirToLight, uNormal),0) * attenuation * intensity;
//     vec3 diffColor = diffIntensity * light.color * texture(material.texture_diffuse0,fs_in.TexCoords).rgb;

//     //specular
//     vec3 halfwayDir = normalize(light.dirToLight + dirToCamera);
//     float specIntensity = pow(max(0,dot(uNormal,halfwayDir)), material.shininess) * intensity;
//     vec3 specularColor = specIntensity * light.color * texture(material.texture_specular0,fs_in.TexCoords).rgb * attenuation;
//     return diffColor + specularColor;
// }