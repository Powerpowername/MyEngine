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
#include "glad/glad.h"
#include "GLFW/glfw3.h"
using glm::vec2;using glm::vec3;using glm::vec4;using glm::mat3;using glm::mat4;
using glm::radians;
using std::vector;
using std::list;
using std::string;
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
#pragma endregion

#pragma region 外部变量
extern GLFWwindow* window;
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
    Camera();
    virtual ~Camera() override;
    virtual void Update() override;
    virtual void OnGUI() override;
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
	CameraMove();
	virtual ~CameraMove()override;
};
#pragma endregion

#pragma region Transform:MonoBehavior
//现在的代码思路给我的感觉是Transform必须是最后一个更新的
//不是最后一个更新的会差一帧，影响应该不大
class Transform : public MonoBehavior
{
public:
    //Transform::Update()修改
    vec3 Forward;
    vec3 Right;
    vec3 Up;

public:
    //由Camera::Update()修改，其他操作对象，按需修改
	vec3 position = vec3(0, 0, 0);//对象的位置
	vec3 rotation = vec3(0, 0, 0);//旋转角度，x为轴的旋转角度，以此类推
	vec3 scale = vec3(1, 1, 1);//对象的缩放比列

    //由CameraMove::Update()修改，其他操作对象，按需修改
    vec3 WorldUp = vec3(0, 1.0f, 0);
	float Pitch = .0f;
	float Yaw = .0f;

	void Translate(vec3 movement);//位移
    mat4 GetModelMaterix(mat4 world = mat4(1.0f))const;//获取对象model矩阵，主要是用来将对象转换到世界坐标系用的

    virtual void OnGUI() override;
    virtual void Update() override;//更新对象自身的坐标系
    Transform(vec3 pos = vec3(0, 0, 0), vec3 rotation = vec3(0, 0, 0), vec3 scale = vec3(1, 1, 1));
    virtual ~Transform() override;
};
#pragma endregion

#pragma region GameObject:Object
class GameObject : public Object
{
public:
    int id;//全局唯一ID
	enum Type
	{
		Empty,
		Cameras,
		Directional,
		Spot,
		Point,
		Box,
		Model
	};
    GameObject* Parent;//暂定保留
    list<MonoBehavior*>* scripts;//控制的行为脚本

    //function
    Transform* transform() const;
    bool enable = true;
    void OnGUI()override;

    template<typename T>
	T* AddComponentStart();//这个stat可能需要调整，因为现在我已经删除了组件的所有Start函数
	template<typename T>
	T * AddComponentNoStart();//NoStart版本的我感觉可能不需要
	template<typename T>
	T* AddComponentStart(MonoBehavior* component);
	template<typename T>
	T* GetComponent()const;
	template<typename T>
	void RemoveComponent();

    GameObject(string name, Type type);
	GameObject(string name, string modelName, string shaderSign);
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
	static bool* Key;
	static bool* keyDown;
	static bool* keyUp;
	static glm::vec2 mouseMentDelta;
	static bool GetKeyDown(int key);
	static bool GetKey(int key);
	static bool GetKeyUp(int key);
	static void GetInput();
	static void ClearInputEveryFrame();
	static void InitInput();
};
#pragma endregion

#pragma region Setting
class Setting
{
public:
    const static string settingDir;//暂定为shader的配置目录等，后面改
    static Camera* MainCamera;
	static std::vector<AbstractLight*>* lights;
	static std::list<GameObject*>* gameObjects;
	static vec2 windowSize;
	static float deltaTime;

    
	static bool lockMouse;

	static int LightCount(AbstractLight::LightType type);


	static void InitSettings();
};
#pragma endregion