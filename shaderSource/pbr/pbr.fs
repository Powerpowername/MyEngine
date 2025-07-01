#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D albedoMap;//基础反射颜色
uniform sampler2D normalMap;//法线贴图·
uniform sampler2D metallicMap;//金属度贴图
uniform sampler2D roughnessMap;//粗糙度贴图
uniform sampler2D aoMap;//环境光遮蔽贴图

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;//此处为切线空间的法线
    //计算TBN矩阵
    vec3 Q1 = dFdx(WorldPos);
    vec3 Q2 = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);
    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)//法线分布函数Trowbridge-Reitz GGX
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N,H),0.0);
    float NdotH2 = NdotH*NdotH;
    
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);//分母
    denom = PI * denom * denom;

    return a2 / denom;
}
//史密斯法的几何分布函数
float GeometrySchlickGGX(float NdotV, float roughness)//几何分布函数Schlick-GGX
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;//此处为直接光照，IBL关照为float k = (roughness*roughness) / 2.0;
    float numerator  = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return numerator / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)//史密斯法处理视线与光线的融合
{
    float NdotV = max(dot(N, V), 0.0);//视线方向
    float NdotL = max(dot(N, L), 0.0);//法线方向
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}
//菲涅尔函数（算反射比率）
vec3 fresnelSchlick(float cosTheta, vec3 F0)//F0为颜色，返回的也是三原色分别的反射比率
{
    //cosTheta是表面法向量n与观察方向v的点乘的结果
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao        = texture(aoMap, TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); //
    F0 = mix(F0, albedo, metallic);//线性插值x+a⋅(y−x)
    vec3 Lo = vec3(0.0);//从表面点射向观察者的总光线辐射率（RGB向量）
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        vec3 kD = vec3(1.0) - kS;//ks是描述物体反射光的能力的一个基础量，会根据物体的材质不同而变化的量，而F是计算物体在不同角度想被观察时的反射比率，
        

    }
}