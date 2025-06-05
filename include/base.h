#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS // 必须在 #include "imgui.h" 之前
#define IMGUI_IMPL_OPENGL_LOADER_GLAD // 告诉 ImGui 使用 GLAD
#define GLM_ENABLE_EXPERIMENTAL 
// #pragma region 预处理
#define GLEW_STATIC
#define var auto

#define KEYS 100
#define W_ 0
#define A_ 1
#define S_ 2
#define D_ 3
#define Q_ 4
#define E_ 5
#define Shift_ 6
#define Space_ 7

#include <iostream>
#include <stdio.h>
#include <memory>
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
#include <stb_image.h>
using glm::vec2;using glm::vec3;using glm::vec4;using glm::mat3;using glm::mat4;
using glm::radians;
using std::vector;
using std::list;
using std::string;
using nlohmann::json;

enum ContorlObject
{
    CAMERA = 0,

};

class Object
{
public:
    string name;


    //function
    Object(string name) : name(name){};
    // virtual void OnGUI() = 0;
    // virtual ~Object();//纯虚函数不可以有定义，但是纯虚析构函数是必须要有定义的，因为没有定义的话，子类在析构时就有可能无法正确析构父类的数据,而且纯虚函数必须在类外实现
};

#pragma region Transform:Object
class Transform : public Object
{
private:
    Object* Parent = nullptr;//Transform必须归属一个可控对象
    vec3 WorldUp = vec3(0, 1.0f, 0);

public:
    //欧拉角，与位移属性由CameraMove::Update修改，其他对象，按需修改
    vec3 position = vec3(0,0,3);//对象的位置
    vec3 rotation = vec3(0,0,0);//旋转角度，分别对应x，y，z，暂时是针对光源的变化
    vec3 scale = vec3(1, 1, 1);//对象的缩放比列

    float Pitch = 0.0f;
	float Yaw = -90.0f;

    //对象自身坐标系由Transform::Update()修改
    vec3 Forward = vec3(0,0,1);
    vec3 Right;
    vec3 Up;

    void Translate(vec3 movement);//位移
    mat4 GetModelMaterix(mat4 world = mat4(1.0f))const;//获取对象的世界坐标
    void OnGUI();
    void Update();//重新计算自身坐标系
    void setParent(Object* Parent);
    Transform(Object* Parent);
    virtual ~Transform() ;
};

class Camera : public Object
{
public:
    vec4 viewPort = vec4(0,0,800,600);//摄像机视口，渲染到窗口的区域
    float Zoom = 45.0f;//摄像机的视口
    float near = 0.1f;//近平面
    float far = 100.0f;//远平面
    std::unique_ptr<Transform> transform;//摄像机的坐标变换
    //将之前从全局定义的移动到了Camra类内,这样设计是为了在Setting类中可以直接修改MainCamera
    //还有一点需要考虑，如果要做阴影是不是放在Transform里更好
    mat4 viewMat;
    mat4 projMat;

    float currentSpeed = 1.0f;
	float sencity = 0.006f;
	float normalSpeed = 5.0f;
	float highSpeed = 15.0f;

    Camera(string name);
    virtual ~Camera() ;
    void Update();
    void OnGUI();
};
#pragma endregion

#pragma region Setting
//全局控制
class Setting
{
public:
    static glm::vec2 pWindowSize;//真正的窗口尺寸
    static GLFWwindow* window;
    //工作目录
    static string workDir;
    static Camera* MainCamera;
    static std::vector<std::list<Object*>> GloabContorl;//控制全局所有可控对象，使用枚举量查找
    static float deltaTime;
	static bool lockMouse;
    virtual void avoidInstance() = 0;//给一个纯虚函数，防止被实例化
};
#pragma endregion

#pragma region Input
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
    // void AddLight(AbstractLight* light);
    // void OnGUI() override;
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

#pragma region 测试
void RenderBox(Shader shader,unsigned int& quadVAO,unsigned int& quadVBO);

#pragma endregion


#pragma region 窗口事件及图像处理模块
unsigned int loadTexture(const char*  path,bool reverse);

#pragma endregion