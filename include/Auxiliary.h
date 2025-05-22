#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS // 必须在 #include "imgui.h" 之前
#define IMGUI_IMPL_OPENGL_LOADER_GLAD // 告诉 ImGui 使用 GLAD
#define GLM_ENABLE_EXPERIMENTAL 
#pragma region 预处理
#define GLEW_STATIC
#define var auto
#include <iostream>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
#include "imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "json.hpp"
#include<fstream>
#include<sstream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
using glm::vec2;using glm::vec3;using glm::vec4;using glm::mat3;using glm::mat4;
using glm::radians;
using std::vector;
using std::list;
using std::string;
using nlohmann::json;
#pragma region 前向声明
class Object;
class MonoBehavior;
class GameObject;
class Camera;
class CameraMove;
class Transform;
class Input;
class Setting;
class AbstractLight;
class LightDirectional;
class LightPoint;
class LightSpot;
class SkyboxRender;
class ModelRender;
class Shader;
#pragma endregion

#pragma region 外部变量
extern GLFWwindow* window;
#pragma endregion

#pragma region 宏
#define GameObjectNum 10
#pragma endregion

#pragma region Object
class Object
{
public:
    string name;


    //function
    Object(string name) : name(name){};
    virtual void OnGUI() = 0;
    virtual ~Object();//纯虚函数不可以有定义，但是纯虚析构函数是必须要有定义的，因为没有定义的话，子类在析构时就有可能无法正确析构父类的数据,而且纯虚函数必须在类外实现
};
#pragma endregion

#pragma region MonoBehavior:Object
class MonoBehavior : public Object
{
public:
    bool enable = true;
    GameObject* gameObject;//所有的抽象行为类都必须归属于一个可操作的对象

    //function
    MonoBehavior();
    virtual ~MonoBehavior() override;
    virtual void Update();//更新自身数据
    virtual void OnGUI() override;
};
#pragma endregion

#pragma region Camera:MonoBehavior
class Camera : public MonoBehavior
{
public:
    vec4 viewPort;//摄像机视口，渲染到窗口的区域
    float angle;//摄像机的视野角度
    float near;//近平面
    float far;//远平面
    Transform* transform;//摄像机的坐标变换
    //将之前从全局定义的移动到了Camra类内,这样设计是为了在Setting类中可以直接修改MainCamera
    //还有一点需要考虑，如果要做阴影是不是放在Transform里更好
    mat4 viewMat;
    mat4 projMat;

    //function
    Camera(GameObject* Parent);
    virtual ~Camera() override;
    virtual void Update() override;
    virtual void OnGUI() override;
	void Start();
};
#pragma endregion

#pragma region CameraMove:MonoBehavior
class CameraMove : public MonoBehavior
{
public:
    float currentSpeed = 1.0f;
    Transform* transform;
	float sencity = 0.006f;
	float normalSpeed = 5.0f;
	float highSpeed = 15.0f;

    //function
    virtual void Update() override;
	virtual void OnGUI() override;
	CameraMove(GameObject* Parent);
	virtual ~CameraMove()override;
};
#pragma endregion

#pragma region Transform:MonoBehavior
class Transform : public MonoBehavior
{
public:
    //欧拉角，与位移属性由CameraMove::Update修改，其他对象，按需修改
	vec3 position = vec3(0, 0, 0);//对象的位置
	vec3 rotation = vec3(0, 0, 0);//旋转角度，分别对应x，y，z，暂时是针对光源的变化
	vec3 scale = vec3(1, 1, 1);//对象的缩放比列
    vec3 WorldUp = vec3(0, 1.0f, 0);
	float Pitch = .0f;
	float Yaw = .0f;
    //对象自身坐标系由Transform::Update()修改
    vec3 Forward;
    vec3 Right;
    vec3 Up;



	void Translate(vec3 movement);//位移
    mat4 GetModelMaterix(mat4 world = mat4(1.0f))const;//获取对象的世界坐标

    void OnGUI() override;
    void Update() override;
    Transform(GameObject* Parent);
    virtual ~Transform() override;
};
#pragma endregion

#pragma region GameObject:Object
class GameObject : public Object
{
private:
    static int idS;
public:
    int id;//全局唯一ID
	enum Type
	{
		GameObject_Empty,
		GameObject_Camera,
		GameObject_Directional,
		GameObject_Spot,
		GameObject_Point,
		GameObject_SkyBox,
		GameObject_Model
	} type;
    GameObject* Parent;//暂定保留
    list<MonoBehavior*>* scripts;//控制的行为脚本

    //function
    Transform* transform() const;
    bool enable = true;
    void OnGUI()override;

    template<typename T>
	T* AddComponent();
	template<typename T>
	T* AddComponent(MonoBehavior* component);//添加现有组件
	template<typename T>
	T* GetComponent()const;
	template<typename T>
	void RemoveComponent();

    GameObject(string name, Type type);
	GameObject(string name, string modelName, string shaderSign);//模型对象的专属构造，后续在看看
	~GameObject();
};
#pragma endregion

#pragma region Input

#define KEYS 100
#define W_ 0
#define A_ 1
#define S_ 2
#define D_ 3
#define Q_ 4
#define E_ 5
#define Shift_ 6
#define Space_ 7

class Input 
{
public:
	static bool* lastKey;
	static bool* Key;//可以用于是否持续按下
	static bool* keyDown;
	static bool* keyUp;
	static glm::vec2 mouseMentDelta;
	static bool GetKeyDown(int key);
	static bool GetKey(int key);
	static bool GetKeyUp(int key);
	static void GetInput();
	static void ClearInputEveryFrame();
	static void InitInput();//应为我这边暂时不准备构造Input的实列，所以就没有显示声明Input的构造函数
};
#pragma endregion



#pragma region AbstractLight:MonoBehavior
class AbstractLight : public MonoBehavior
{
public:
	Transform* transform;
	virtual string Sign()const = 0;//返回光的类型
	vec3 direction;//光源的方向是从光源在世界坐标系的z轴正方向开始Update()，函数可以看出是每次都是从这个方向开始变换
	vec3 color;
	void virtual FromJson(const json& j);
	void virtual ToJson(json& j)const;

public:
	enum class LightType//添加了class的枚举类型就不能隐式转换为整数了
	{
		Directional,
		Spot,
		Point
	};

	float strength = 1.0f;//光强
	virtual LightType Type() const = 0;
	AbstractLight();
	virtual ~AbstractLight() override;
	void OnGUI() override;
	void Update()override;
    virtual void SetShader(Shader* shader, int index);//需要知道光源在着色器的uniform的什么位置才好设置
	friend void to_json(json& j, const AbstractLight& l);
	friend void from_json(const json& j, AbstractLight& l);
};

#pragma endregion















#pragma region Shader:Object
class Shader : public Object
{
public:
    unsigned int ID;
    const int maxImage = 4;

    int currentImages = 0;
	std::string vertexString;
	std::string fragmentString;
	const char* vertexSource;
	const char* fragmentSource;

    //function
    Shader(string shaderName,string shaderPath, const char* geometryPath = nullptr);
    void use();
    void AddLight(AbstractLight* light);
    void OnGUI() override;
	void setBool(const std::string & name, bool value) const;
	void setInt(const std::string & name, int value) const;
	void setFloat(const std::string & name, float value) const;
	void setVec2(const std::string & name, const glm::vec2 & value) const;
	void setVec2(const std::string & name, float x, float y) const;
	void setVec3(const std::string & name, const glm::vec3 & value) const;
	void setVec3(const std::string & name, float x, float y, float z) const;
	void setVec4(const std::string & name, const glm::vec4 & value) const;
	void setVec4(const std::string & name, float x, float y, float z, float w);
	void setMat2(const std::string & name, const glm::mat2 & mat) const;
	void setMat3(const std::string & name, const glm::mat3 & mat) const;
	void setMat4(const std::string & name, const glm::mat4 & mat) const;

private:
    void checkCompileErrors(GLuint shader, std::string type);
    int Light_num = 0;
};


#pragma endreigon


#pragma region Setting
class Setting
{
public:
    //窗口属性从全局变量变为Setting的属性
    static glm::vec2 pWindowSize;//真正的窗口尺寸
    static GLFWwindow* window;
    static mat4 modelMat;
    static mat4 viewMat;
    static mat4 projMat;
    //工作目录
    static string workDir;


    const static string settingDir;//暂定为shader的配置目录等，后面改
    // static Camera* MainCamera;
    static GameObject* MainCamera;
	// static std::vector<AbstractLight*>* lights;
	static std::list<GameObject*> gameObjects[GameObjectNum];//这边我准备将他划分位指向不同链表的指针
	// static vec2 windowSize;//暂定认为是渲染窗口
	static float deltaTime;

    
	static bool lockMouse;

	static int LightCount(AbstractLight::LightType type);//返回指定光源的数量


	// static void InitSettings();
};
#pragma endregion




#pragma region json
//添加json对glm适配的模板
namespace nlohmann
{
	template<>
	struct adl_serializer<glm::vec3>
	{
		static void to_json(json& j, const glm::vec3 & v)
		{
			j = json{
				{"x",v.x},
			{"y",v.y},
			{"z",v.z}
			};
		}
		static void from_json(const json& j, glm::vec3 & v)
		{
			v = glm::vec3(
				j.at("x").get<float>(),
				j.at("y").get<float>(),
				j.at("z").get<float>()
			);
		}
	};
}
#pragma endregion