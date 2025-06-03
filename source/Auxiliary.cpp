#include "Auxiliary.h"

Object::~Object()
{
}
//子类会自动调用父类的析构函数
MonoBehavior::MonoBehavior() : Object("MonoBehavior_")
{
    this->gameObject = nullptr;
}

MonoBehavior::~MonoBehavior()
{

}

void MonoBehavior::Update()
{
}

void MonoBehavior::OnGUI()
{
    ImGui::Checkbox("Enable",(bool*)&enable);
}

Camera::Camera(GameObject* Parent)
{
    name += "Camera";
	this->gameObject = Parent;
    transform = gameObject->transform();//GameObject在他继承得MonoBehavior中
	// std::cout<<"Camera"<<std::endl;
	
	transform->Pitch = radians(15.0f);
	transform->Yaw = radians(180.0f);
	viewPort = vec4(0, 0, Setting::pWindowSize.x, Setting::pWindowSize.y);//暂时不修改，看看后期有没有优化的办法，放到Setting里是不是更好
}

Camera::~Camera()
{
}

void Camera::Start()
{
	
}
void Camera::Update()
{
    glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);//渲染的窗口
	viewMat = lookAt(transform->position, transform->position + transform->Forward, transform->WorldUp);
	projMat = glm::perspective(radians(this->angle), viewPort.z / viewPort.w, near, far);
}
//这边的考虑是将修改值得操作和更新Camera本身得属性分离开
void Camera::OnGUI()
{
    MonoBehavior::OnGUI();//有Enable的checkbox要用到
	ImGui::DragFloat4(("viewPort" + std::to_string(gameObject->id)).c_str(), (float*)&viewPort, 10, 0, 2000);
	ImGui::DragFloat(("viewAngle" + std::to_string(gameObject->id)).c_str(), (float*)&angle, 3, 0, 180.0f);
	ImGui::DragFloat(("near" + std::to_string(gameObject->id)).c_str(), (float*)&near, 0.01f, 0, 10);
	ImGui::DragFloat(("far" + std::to_string(gameObject->id)).c_str(), (float*)&far, 1.0f, 0, 1000);
}

CameraMove::CameraMove(GameObject* Parent)
{
	this->gameObject = Parent;
	name += "CameraMove";
    transform = gameObject->transform();
}

void CameraMove::Update()
{
	MonoBehavior::Update();
    //摄像机移动速度
	if (Input::GetKeyDown(Shift_))
		currentSpeed = highSpeed;
	if (Input::GetKeyUp(Shift_))
		currentSpeed = normalSpeed;

    //键盘输出控制
    if(Input::GetKey(S_))
        transform->Translate(-currentSpeed * transform->Forward*Setting::deltaTime);//Translate的参数是矢量
	if (Input::GetKey(W_))
		transform->Translate(currentSpeed * transform->Forward*Setting::deltaTime);
	if (Input::GetKey(A_))
		transform->Translate(-currentSpeed * transform->Right*Setting::deltaTime);
	if (Input::GetKey(D_))	
		transform->Translate(currentSpeed*transform->Right*Setting::deltaTime);
	if (Input::GetKey(Q_))
		transform->Translate(-currentSpeed * transform->Up*Setting::deltaTime);
	if (Input::GetKey(E_))
		transform->Translate(currentSpeed*transform->Up*Setting::deltaTime);

    //是否锁定鼠标，就是看不到鼠标了
    if (Input::GetKeyDown(Space_))
	{
		Setting::lockMouse = !Setting::lockMouse;
		if (Setting::lockMouse)
			glfwSetInputMode(Setting::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(Setting::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

    //没有锁定鼠标就返回，只有锁定鼠标后才能更改Camera的Pitch和Yaw
    if (!Setting::lockMouse)
        return;
	transform->Pitch -= Input::mouseMentDelta.y * sencity;
	transform->Yaw -= Input::mouseMentDelta.x * sencity;
}

void CameraMove::OnGUI()
{
    //与camera的GUI同理先修改，再更新
	MonoBehavior::OnGUI();
	ImGui::DragFloat("sencity", (float*)&sencity, 0.001f, 0, 0.010f);
	ImGui::DragFloat("normalSpeed", (float*)&normalSpeed, 1, 1, 20);
	ImGui::DragFloat("highSpeed", (float*)&highSpeed, 1, 5, 50);
}

CameraMove::~CameraMove()
{
}
#pragma region Transform:MonoBehavior

void Transform::Translate(vec3 movement)
{
    position += movement;
}

mat4 Transform::GetModelMaterix(mat4 world) const
{
    auto model = glm::translate(world,position);
    model = glm::scale(model,scale);
	model = rotate(model, radians(rotation.x), vec3(1, 0, 0));
	model = rotate(model, radians(rotation.y), vec3(0, 1, 0));
	model = rotate(model, radians(rotation.z), vec3(0, 0, 1));
	return model;
}

void Transform::OnGUI()
{
    MonoBehavior::OnGUI();
	ImGui::DragFloat3((gameObject->name + "_position").c_str(), (float*)&position, 0.5f, -500, 500);
	ImGui::DragFloat3((gameObject->name + "_rotation").c_str(), (float*)&rotation, 0.5f, -500, 500);
	ImGui::DragFloat3((gameObject->name + "_scale").c_str(), (float*)&scale, 0.01f, -100, 100);
	ImGui::DragFloat((gameObject->name + "_Yaw").c_str(), (float*)&Yaw, 0.01f, -10, 10);
	ImGui::DragFloat((gameObject->name + "_Pitch").c_str(), (float*)&Pitch, 0.01f, -10, 10);
}

void Transform::Update()
{
	Forward.x = cos(Pitch)*sin(Yaw);
	Forward.y = sin(Pitch);
	Forward.z = cos(Pitch)*cos(Yaw);

	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Forward, Right));
}

Transform::Transform(GameObject* Parent)
{
	this->gameObject = Parent;
	name += "Transform";
	Forward.x = cos(Pitch)*sin(Yaw);
	Forward.y = sin(Pitch);
	Forward.z = cos(Pitch)*cos(Yaw);

	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Right, Forward));
}
Transform::~Transform()
{
}
#pragma endregion

#pragma region GameObject : Object
template<typename T>
T* GameObject::AddComponent()
{

	auto mb = (MonoBehavior*)new T(this);//指向的是行为脚本
	// mb->gameObject = this;
	scripts->push_back(mb);

	return (T*)mb;//没人接收也没关系，由scripts管理声明周期
}
template <typename T>
T* GameObject::AddComponent(MonoBehavior *component)
{
	component->gameObject = this;
	scripts->push_back(component);
	return (T*) component;
}
template <typename T>
T* GameObject::GetComponent() const
{
	for(auto mb : *scripts)
		if(typeid(*mb) == typeid(T))
			return (T*)mb;
	return nullptr;
}

template<typename T>
void  GameObject::RemoveComponent()
{
	for (auto x : *scripts)
		if(typeid(*x)==typeid(T))
			scripts->remove(x);
}

Transform * GameObject::transform() const
{
	return GetComponent<Transform>();
}

void GameObject::OnGUI()
{
	ImGui::Checkbox("Enable",(bool*)&enable);//后期考虑在每一个组件或者对象后面加一个delete属性，用来删除对象或组件
	if (this->type == GameObject_Camera)//只有摄像机才有下面的操作
	{
		if (ImGui::Button("Go to here"))
		{
			Setting::MainCamera = this;
		}
	}
}
GameObject::GameObject(string name, string modelName, string shaderSign) :Object("GameObject_" + name)
{
	// this->id = idS++;
	// this->type = GameObject_Model;

	// Setting::gameObjects->push_back(this);
	// scripts = new std::list<MonoBehavior*>();
	// AddComponent<Transform>();
	// auto modelRender = AddComponent<ModelRender>();
	// modelRender->modelName = modelName;
}
int GameObject::idS = 0;
GameObject::GameObject(string name, Type type) :Object("GameObject_" + name)
{
	
	this->id = idS++;
	scripts = new std::list<MonoBehavior*>();
	AddComponent<Transform>();
	this->type = type;
	switch(type)
	{
	case GameObject_Camera:
		AddComponent<Camera>();//相机对象添加相机组件
		if(!Setting::MainCamera)//无主相机就添加主相机
		{
			Setting::MainCamera = this;
			// std::cout<<"sk1"<<std::endl;
		}
		AddComponent<CameraMove>();
		// std::cout<<"sk2"<<std::endl;
		Setting::gameObjects[GameObject_Camera].push_back(this);//0为摄像机所管理的对象
		// std::cout<<"sk3"<<std::endl;
		break;
	// case GameObject_Directional:
	// 	AddComponent<LightDirectional>();
	// 	Setting::gameObjects[GameObject_Directional].push_back(this);
	// 	break;
	// case GameObject_Spot:
	// 	AddComponent<LightPoint>();
	// 	Setting::gameObjects[GameObject_Spot].push_back(this);
	// 	break;
	// case GameObject_Point:
	// 	AddComponent<LightSpot>();
	// 	Setting::gameObjects[GameObject_Point].push_back(this);
	// 	break;
	// case GameObject_SkyBox:
	// 	AddComponent<SkyboxRender>();
	// 	Setting::gameObjects[GameObject_SkyBox].push_back(this);
	// 	break;
	// case GameObject_Model:
	// 	AddComponent<ModelRender>();
	// 	Setting::gameObjects[GameObject_Model].push_back(this);
	// 	break;
	case GameObject_Empty:
		break;
	default:
		break;
	}

}
GameObject::~GameObject()//释放对象scripts的所有操作，并最终释放scripts自身
{
	for(auto x : *scripts)
		delete x;
	delete scripts;
}
#pragma endregion

#pragma region
bool* Input::keyDown = new bool[KEYS];
bool* Input::Key = new bool[KEYS];
bool* Input::lastKey = new bool[KEYS];//上一帧按键状态
bool* Input::keyUp = new bool[KEYS];

glm::vec2 Input::mouseMentDelta = glm::vec2(0,0);//鼠标移动增量

bool Input::GetKey(int key)
{
	return Key[key];
}

bool Input::GetKeyUp(int key)
{
	return keyUp[key];
}

bool Input::GetKeyDown(int key)
{
	return keyDown[key];
}

void Input::GetInput()//这边目前只能看出一帧内按键是否按下，持续按下的情况我现在看来是只会被认为按下一次，后续都被认为是没按下
{
	for(int i = 0;i < KEYS;i++)
	{
		if (Key[i] && !lastKey[i])//上一帧没按下，当前帧按下
			keyDown[i] = true;
		if (!Key[i] && lastKey[i])//上一帧按下，当前没帧按下
			keyUp[i] = true;
	}
}

void Input::ClearInputEveryFrame()
{
	memcpy(lastKey,Key,sizeof(bool)*KEYS);
	memset(Key,0,sizeof(bool)*KEYS);
	memset(keyDown, 0, sizeof(bool)*KEYS);
	memset(keyUp, 0, sizeof(bool)*KEYS);
	mouseMentDelta = vec2(0, 0);
}

void Input::InitInput()
{
	memset(Key, 0, sizeof(bool)*KEYS);
	memset(keyDown, 0, sizeof(bool)*KEYS);
	memset(keyUp, 0, sizeof(bool)*KEYS);
	memset(lastKey, 0, sizeof(bool)*KEYS);
}
#pragma endregion

#pragma region Setting
// std::vector<AbstractLight*>* Setting::lights = nullptr;
std::list<GameObject*> Setting::gameObjects[GameObjectNum];
vec2 Setting::pWindowSize = vec2(1200, 1000);
GLFWwindow* Setting::window =nullptr;
mat4 Setting::modelMat;
mat4 Setting::viewMat;
mat4 Setting::projMat;
string Setting::workDir = "E:\\GitStore\\MyEngine\\MyEngine";
string const Setting::settingDir = workDir + "\\settings";
GameObject* Setting::MainCamera = nullptr;
// vec2 Setting::windowSize = vec2(1200, 1000);
float Setting::deltaTime = 0.02f;//不是应该是0吗？
bool Setting::lockMouse = false;

int Setting::LightCount(AbstractLight::LightType type)
{
	if(type == AbstractLight::LightType::Directional)
		return gameObjects[GameObject::GameObject_Directional].size();
	if(type == AbstractLight::LightType::Spot)
		return gameObjects[GameObject::GameObject_Spot].size();
	if(type == AbstractLight::LightType::Point)
		return gameObjects[GameObject::GameObject_Point].size();
}

// void Setting::InitSettings()//似乎用不上了
// {
// 	// lights = new std::vector<AbstractLight*>();
// 	// gameObjects = new std::list<GameObject*>(10);
// }
#pragma endregion

#pragma region AbstractLight:MonoBehavior
void AbstractLight::Update()
{
	direction = vec3(0,0,1.0f);
	direction = glm::rotateZ(direction, transform->rotation.z);
	direction = glm::rotateX(direction, transform->rotation.x);
	direction = glm::rotateY(direction, transform->rotation.y);
	direction *= -1;
}

void AbstractLight::ToJson(json & j)const
{
	j = json{
	{"direction",direction},
	{"color",color},
	{"strength",strength}
	};
}

void AbstractLight::FromJson(const json & j)
{
	color = j.at("color").get<vec3>();
	direction = j.at("direction").get<vec3>();
	strength = j.at("strength").get<float>();
}

AbstractLight::AbstractLight() : color(vec3(1,1,1))
{
	// transform = gameObject->transform();
	// Setting::lights->push_back(this);
}

AbstractLight::~AbstractLight()
{

}

void AbstractLight::OnGUI() 
{
	MonoBehavior::OnGUI();
	ImGui::SliderFloat("DirStrength", (float*)&strength, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::DragFloat3("DirAngles", (float*)&transform->rotation, 0.1f, 0, 2 * 3.1415926535f);
	ImGui::ColorEdit3("LightColor", (float*)&color); // Edit 3 floats representing a color
}

void AbstractLight::SetShader(Shader* shader, int index)
{
	shader->setBool(Sign() + "[" + std::to_string(index) + "]." + "flag", true);
	shader->setVec3(Sign() + "[" + std::to_string(index) + "]." + "color", this->color*strength);
	shader->setVec3(Sign() + "[" + std::to_string(index) + "]." + "pos", transform->position);
	shader->setVec3(Sign() + "[" + std::to_string(index) + "]." + "dirToLight", this->direction);
}

void to_json(json & j, const AbstractLight & l)
{
	l.ToJson(j);
}

void from_json(const json & j, AbstractLight & l)
{
	l.FromJson(j);
}

#pragma endregion

#pragma region LightDirectional:AbstractLight
string LightDirectional::Sign() const
{
    return "lightDirectional";
}
LightDirectional::LightDirectional(GameObject* Parent)
{
	//会隐式调用父类的构造函数
	name += "DirectionalLight";
	this->gameObject = Parent;
	transform = gameObject->transform();
	transform->position = vec3(0,0,0);
	transform->rotation = vec3(radians(45.0f), radians(90.0f), radians(0.0f));//旋转角度，分别对应x，y，z，暂时是针对光源的变化
	// color = vec3(1,1,1);//父类构造函数已经实现了
}

// AbstractLight::LightType LightDirectional::Type()const 
// {
// 	return LightType::Directional;
// }

LightDirectional::~LightDirectional()
{
}

#pragma  endregion

#pragma region LightPoint:AbstractLight
LightPoint::LightPoint(GameObject* Parent)
{
	//同样隐式调用父类构造函数,除非有多个构造函数要用特定的再显示调用
	name += "PointLight";
	this->gameObject = Parent;
	transform = gameObject->transform();
	transform->position = vec3(0,0,0);
	transform->rotation = vec3(radians(45.0f), radians(90.0f), radians(0.0f));//旋转角度，分别对应x，y，z，暂时是针对光源的变化

}
string LightPoint::Sign() const
{
	return "lightPoint";
}
void LightPoint::SetShader(Shader *shader,int index)
{
	AbstractLight::SetShader(shader,index);
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "constant",this->constant);
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "linear", this->linear);
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "quadratic", this->quadratic);
}

void LightPoint::OnGUI() 
{
	AbstractLight::OnGUI();
	ImGui::DragFloat("constant", (float*)&constant, 0.01f, 0, 2);
	ImGui::DragFloat("linear", (float*)&linear, 0.1f, 0, 10);
	ImGui::DragFloat("quadratic", (float*)&quadratic, 0.01f, 0, 0.5f);
}

void LightPoint::FromJson(const json & j)
{

	AbstractLight::FromJson(j);
	constant = j.at("constant").get<float>();
	linear = j.at("linear").get<float>();
	quadratic = j.at("quadratic").get<float>();
}
void LightPoint::ToJson(json & j) const
{
	AbstractLight::ToJson(j);
	j.push_back({ "constant",constant });
	j.push_back({ "linear" ,linear });
	j.push_back({ "quadratic",quadratic });
}

LightPoint::~LightPoint()
{

}
#pragma endregion

#pragma region LightSpot:LightPoint
LightSpot::LightSpot(GameObject* Parent) : LightPoint(Parent)
{
	name = "MonoBehavior_SpotLight";
	transform = Parent->transform();
	transform->position = vec3(0, 0, 0);
	transform->rotation = vec3(radians(90.0f), radians(0.0f), radians(0.0f));
}

void LightSpot::OnGUI() 
{
	LightPoint::OnGUI();
	ImGui::DragFloat("cosPhyInner", (float*)&cosPhyInner, 0.01f, 0, 1);
	ImGui::DragFloat("cosPhyOuter", (float*)&cosPhyOuter, 0.01f, 0, 1);
}

LightSpot::~LightSpot()
{
}

void LightSpot::FromJson(const json & j)
{
	LightPoint::FromJson(j);
	cosPhyInner = j.at("cosPhyInner").get<float>();
	cosPhyOuter = j.at("cosPhyOuter").get<float>();
}

void LightSpot::ToJson(json & j) const
{
	LightPoint::ToJson(j);
	j.push_back(
		{ "cosPhyInner",cosPhyInner }
	);
	j.push_back(
		{ "cosPhyOuter",cosPhyOuter });
}

void LightSpot::SetShader(Shader * shader, int index)
{
	LightPoint::SetShader(shader, index);

	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "cosPhyInner", this->cosPhyInner);
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "cosPhyOuter", this->cosPhyOuter);
}
string LightSpot::Sign() const
{
	return "lightSpot";
}
#pragma endregion 





#pragma region Shader:Object
Shader::Shader(string shaderName,string shaderPath, const char* geometryPath) :Object("Shader_" + shaderName)
{
	std::cout << "Shader Name: " << shaderPath << std::endl;
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// open files
		vShaderFile.open(shaderPath + ".vs");
		fShaderFile.open(shaderPath + ".fs");
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != nullptr)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);

}

void Shader::AddLight(AbstractLight* light)
{
	light->SetShader(this,Setting::LightCount(light->Type()) - 1);//这个只会将最后的光源加入到Shader中，所以说是需要注意更新的
}

void Shader::OnGUI()
{
	if(ImGui::TreeNode(name.c_str()))
	{
		ImGui::Text("name: %s", name.c_str());
		ImGui::TreePop();
		ImGui::Spacing();
	}
}

void Shader::use()
{
	glUseProgram(ID);
}
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
#pragma endregion

#pragma region friend json
void to_json(json & j, const LightDirectional & l)
{
	j = json{
	{"direction",l.direction},
	{"color",l.color},
	{"strength",l.strength}
	};
}

void from_json(const json & j, LightDirectional & l)
{
	l.color = j.at("color").get<vec3>();
	l.direction = j.at("direction").get<vec3>();
	l.strength = j.at("strength").get<float>();

}
#pragma endregion


#pragma region 测试
void RenderBox(Shader shader,unsigned int quadVAO,unsigned int quadVBO)
{
	if(quadVAO == 0)
	{
		// positions
        glm::vec3 pos1(-1.0, 1.0, 0.0);
        glm::vec3 pos2(-1.0, -1.0, 0.0);
        glm::vec3 pos3(1.0, -1.0, 0.0);
        glm::vec3 pos4(1.0, 1.0, 0.0);
		// texture coordinates
        glm::vec2 uv1(0.0, 1.0);
        glm::vec2 uv2(0.0, 0.0);
        glm::vec2 uv3(1.0, 0.0);
        glm::vec2 uv4(1.0, 1.0);
        // normal vector
        glm::vec3 nm(0.0, 0.0, 1.0);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // - triangle 1
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // - triangle 2
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);

        GLfloat quadVertices[] = {
            // Positions            // normal         // TexCoords  // Tangent                          // Bitangent
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };

		// Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
#pragma endregion