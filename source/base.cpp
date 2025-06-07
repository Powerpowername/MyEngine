#include <base.h>
#pragma region Transform:Object
void Transform::Translate(vec3 movement)
{
    position += movement;
}

Transform::Transform(Object* Parent) : Object("Transform")
{
    this->Parent = Parent;
    
}

mat4 Transform::GetModelMaterix(mat4 world) const
{
    auto model = glm::translate(world,position);
    model = glm::scale(model,scale);
    //旋转
	model = glm::rotate(model, radians(rotation.x), vec3(1, 0, 0));
	model = glm::rotate(model, radians(rotation.y), vec3(0, 1, 0));
	model = glm::rotate(model, radians(rotation.z), vec3(0, 0, 1));
	return model;
}

void Transform::Rotate()
{
	vec3 axis(0,1,0);
	//需要限制角度大小
	mat4 model = mat3(1);
	position = glm::rotate(model,glm::radians(rotateAngle),axis) * vec4(position,0);//到底更不更新 Update
    
	//只有一个转轴
	Yaw -= rotateAngle;//现在只考虑绕Y轴转


	// Pitch = glm::clamp(Pitch, -89.0f, 89.0f); // 限制俯仰角防止万向节锁
    if (Yaw > 360.0f) Yaw -= 360.0f;
    if (Yaw < 0.0f) Yaw += 360.0f;

}

void Transform::OnGUI()
{
	ImGui::DragFloat3((Parent->name + "_position").c_str(), (float*)&position, 0.5f, -500, 500);
	ImGui::DragFloat3((Parent->name + "_rotation").c_str(), (float*)&rotation, 0.5f, -500, 500);
	ImGui::DragFloat3((Parent->name + "_scale").c_str(), (float*)&scale, 0.01f, -100, 100);
	ImGui::DragFloat((Parent->name + "_Yaw").c_str(), (float*)&Yaw, 0.01f, 0.0f, 360.0f);
	ImGui::DragFloat((Parent->name + "_Pitch").c_str(), (float*)&Pitch, 0.01f, -89, 89);
	ImGui::DragFloat((Parent->name + "_rotateAngle").c_str(), (float*)&rotateAngle, 1.0f, -360, 360);
}

void Transform::Update()
{
	Rotate();
    float pitchRad = glm::radians(Pitch);
    float yawRad = glm::radians(Yaw);

    // 正确计算公式（OpenGL右手坐标系，看向-Z方向）
    Forward.x = cos(yawRad) * cos(pitchRad);
    Forward.y = sin(pitchRad);
    Forward.z = sin(yawRad) * cos(pitchRad);
    
    Forward = glm::normalize(Forward);
    Right = glm::normalize(glm::cross(Forward, WorldUp));
    Up = glm::normalize(glm::cross(Right, Forward));
}
void Transform::setParent(Object* Parent)
{
    this->Parent = Parent;
}
Transform::~Transform()
{

}
#pragma endregion

#pragma region Camera:Object
Camera::Camera(string name) : Object(name)
{
    transform = std::make_unique<Transform>(this);
    transform->Update();
}

Camera::~Camera()
{

}
void Camera::Update()
{
    transform->Update();
    // glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);//渲染的窗口
	viewPort = vec4(0,0,Setting::pWindowSize.x,Setting::pWindowSize.y);
    viewMat = lookAt(transform->position, transform->position + transform->Forward, transform->Up);
	projMat = glm::perspective(radians(Zoom), viewPort.z / viewPort.w, near, far);

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

void Camera::OnGUI()
{
    transform->OnGUI();
	ImGui::DragFloat4((name + "viewPort").c_str(), (float*)&viewPort, 10, 0, 2000);
	ImGui::DragFloat((name +"viewAngle").c_str(), (float*)&Zoom, 3, 0, 180.0f);
	ImGui::DragFloat((name + "near").c_str(), (float*)&near, 0.01f, 0, 10);
	ImGui::DragFloat((name + "far").c_str(), (float*)&far, 1.0f, 0, 1000);
	ImGui::DragFloat((name + "sencity").c_str(), (float*)&sencity, 0.001f, 0, 0.010f);
	ImGui::DragFloat((name + "normalSpeed").c_str(), (float*)&normalSpeed, 1, 1, 20);
	ImGui::DragFloat((name +"highSpeed").c_str(), (float*)&highSpeed, 1, 5, 50);

}
#pragma endregion

#pragma region 光源组
//-------------------------抽象光---------------------
AbstractLight::AbstractLight(vec3 LightColor) : LightColor(LightColor),Object("Light")
{
	//派生类需要重置名称
	transform = std::make_unique<Transform>(this);
    transform->position = vec3(0,0,0);
}

void AbstractLight::OnGUI()
{
    ImGui::DragFloat3((name + "Position").c_str(),(float*)&(transform->position), 0.5f, -500.0f, 500.0f);
    ImGui::DragFloat3((name + "Color").c_str(),(float*)&LightColor,0.1,0,1);//暂时未考虑HDR
    ImGui::DragFloat3((name + "Forward").c_str(),(float*)&(transform->Forward),0.1,0,1);//这是光源的方向向量，目前不打算再光源中加入Transform的Update以减小开销

}

AbstractLight::~AbstractLight()
{

}
//-------------------------平行光---------------------
unsigned int DirctionLight::DirctionLightNum = 0;
DirctionLight::DirctionLight(vec3 LightColor,vec3 Position) : AbstractLight(LightColor)
{
    ID = DirctionLightNum;
    DirctionLightNum++;
    name = "DirctionLight" + std::to_string(ID) + "_";
    transform->position = Position;
}
DirctionLight::~DirctionLight()
{

}

void DirctionLight::setShader(Shader shader)
{
    //启用光源
    shader.setBool("directionLight[" + std::to_string(ID) + "].flag",1);
    shader.setVec3("directionLight[" + std::to_string(ID) + "].color",LightColor);
    shader.setVec3("directionLight[" + std::to_string(ID) + "].pos",transform->position);
    shader.setVec3("directionLight[" + std::to_string(ID) + "].dirToLight",-transform->Forward);
}

void DirctionLight::OnGUI()
{
    ImGui::Checkbox((name + "Show").c_str(), &ShowGUI);
    if(ShowGUI)
    {
        AbstractLight::OnGUI();
    }
}
unsigned int DirctionLight::showID()
{
    return ID;
}
// void DirctionLight::Update()
// {

// }
//-------------------------点光源---------------------
unsigned int PointLight::PotLightNum = 0;
PointLight::PointLight(vec3 LightColor,vec3 Position ) : AbstractLight(LightColor)
{
    ID = PotLightNum;
    PotLightNum++;
    name = "PointLight" + std::to_string(ID) + "_";
    transform->position = Position;
}
PointLight::~PointLight()
{
    ImGui::Checkbox((name + "Show").c_str(), &ShowGUI);
    if(ShowGUI)
    {
        AbstractLight::OnGUI();
        ImGui::DragFloat((name + "Constant").c_str(),(float*)&constant,0.1f,0.0f,2.0f);
        ImGui::DragFloat((name + "Linear").c_str(),(float*)&linear,0.01f,0.0f,2.0f);
        ImGui::DragFloat((name + "Quadratic").c_str(),(float*)&quadratic,0.01f,0.0f,2.0f);
    }
}
void PointLight::setShader(Shader shader)
{
    //启用光源
    shader.setBool("pointLight[" + std::to_string(ID) + "].flag",1);
    shader.setVec3("pointLight[" + std::to_string(ID) + "].pos",transform->position);
    shader.setVec3("pointLight[" + std::to_string(ID) + "].dirToLight",-transform->Forward);
    shader.setVec3("pointLight[" + std::to_string(ID) + "].color",LightColor);
    shader.setFloat("pointLight[" + std::to_string(ID) + "].constant",constant);
    shader.setFloat("pointLight[" + std::to_string(ID) + "].linear",linear);
    shader.setFloat("pointLight[" + std::to_string(ID) + "].quadratic",quadratic);
}

void PointLight::OnGUI()
{
    ImGui::Checkbox((name + "Show").c_str(), &ShowGUI);
    if(ShowGUI)
    {
        AbstractLight::OnGUI();

    }
}

unsigned int PointLight::showID()
{
    return ID;
}
#pragma endregion

#pragma region Setting
//全局控制
glm::vec2 Setting::pWindowSize =vec2(800,600);//真正的窗口尺寸
GLFWwindow* Setting::window = nullptr;
    //工作目录
string Setting::workDir = "";
Camera* Setting::MainCamera = nullptr;
std::vector<std::list<Object*>> Setting::GloabContorl;//控制全局所有可控对象，使用枚举量查找
float Setting::deltaTime = 0;
bool Setting::lockMouse = false;

#pragma endregion

#pragma region Input
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

// void Shader::AddLight(AbstractLight* light)
// {
// 	light->SetShader(this,Setting::LightCount(light->Type()) - 1);//这个只会将最后的光源加入到Shader中，所以说是需要注意更新的
// }

// void Shader::OnGUI()
// {
// 	if(ImGui::TreeNode(name.c_str()))
// 	{
// 		ImGui::Text("name: %s", name.c_str());
// 		ImGui::TreePop();
// 		ImGui::Spacing();
// 	}
// }

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

void RenderBox(Shader shader, unsigned int& cubeVAO, unsigned int& cubeVBO) {
    if (cubeVAO == 0) {
        // 正方体顶点位置（中心在原点，边长2）
        glm::vec3 positions[] = {
            // 前面
            glm::vec3(-1.0f, -1.0f,  1.0f), // 左下前
            glm::vec3( 1.0f, -1.0f,  1.0f), // 右下前
            glm::vec3( 1.0f,  1.0f,  1.0f), // 右上前
            glm::vec3(-1.0f,  1.0f,  1.0f), // 左上前
            
            // 后面
            glm::vec3(-1.0f, -1.0f, -1.0f), // 左下后
            glm::vec3( 1.0f, -1.0f, -1.0f), // 右下后
            glm::vec3( 1.0f,  1.0f, -1.0f), // 右上后
            glm::vec3(-1.0f,  1.0f, -1.0f), // 左上后
        };

        // 每个面的法线方向
        glm::vec3 normals[] = {
            glm::vec3( 0.0f,  0.0f,  1.0f), // 前面
            glm::vec3( 0.0f,  0.0f, -1.0f), // 后面
            glm::vec3(-1.0f,  0.0f,  0.0f), // 左面
            glm::vec3( 1.0f,  0.0f,  0.0f), // 右面
            glm::vec3( 0.0f, -1.0f,  0.0f), // 下面
            glm::vec3( 0.0f,  1.0f,  0.0f)  // 上面
        };

        // 纹理坐标（每个面独立）
        glm::vec2 uvs[] = {
            glm::vec2(0.0f, 0.0f), // 左下
            glm::vec2(1.0f, 0.0f), // 右下
            glm::vec2(1.0f, 1.0f), // 右上
            glm::vec2(0.0f, 1.0f)  // 左上
        };

        // 正方体由6个面组成，每个面2个三角形（6个顶点）
        std::vector<float> cubeVertices;
        
        // 定义6个面的顶点索引[9](@ref)
        unsigned int indices[6][4] = {
            {0, 1, 2, 3}, // 前面
            {5, 4, 7, 6}, // 后面
            {4, 0, 3, 7}, // 左面
            {1, 5, 6, 2}, // 右面
            {4, 5, 1, 0}, // 下面
            {3, 2, 6, 7}  // 上面
        };

        // 处理每个面
        for (int face = 0; face < 6; face++) {
            // 获取当前面的4个顶点
            glm::vec3 v0 = positions[indices[face][0]];
            glm::vec3 v1 = positions[indices[face][1]];
            glm::vec3 v2 = positions[indices[face][2]];
            glm::vec3 v3 = positions[indices[face][3]];
            
            // 当前面的法线
            glm::vec3 nm = normals[face];
            
            // 计算切空间向量（使用第一个三角形）
            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec2 deltaUV1 = uvs[1] - uvs[0];
            glm::vec2 deltaUV2 = uvs[2] - uvs[0];
            
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            
            glm::vec3 tangent, bitangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);
            
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);
            
            // 添加两个三角形（6个顶点）到顶点数组
            for (int i = 0; i < 4; i++) {
                // 位置
                cubeVertices.push_back(positions[indices[face][i]].x);
                cubeVertices.push_back(positions[indices[face][i]].y);
                cubeVertices.push_back(positions[indices[face][i]].z);
                
                // 法线
                cubeVertices.push_back(nm.x);
                cubeVertices.push_back(nm.y);
                cubeVertices.push_back(nm.z);
                
                // 纹理坐标
                cubeVertices.push_back(uvs[i].x);
                cubeVertices.push_back(uvs[i].y);
                
                // 切线
                cubeVertices.push_back(tangent.x);
                cubeVertices.push_back(tangent.y);
                cubeVertices.push_back(tangent.z);
                
                // 副切线
                cubeVertices.push_back(bitangent.x);
                cubeVertices.push_back(bitangent.y);
                cubeVertices.push_back(bitangent.z);
            }
            
            // 添加第二个三角形的额外两个顶点（使用索引2和3）
            for (int i = 2; i < 4; i++) {
                // 位置
                cubeVertices.push_back(positions[indices[face][i]].x);
                cubeVertices.push_back(positions[indices[face][i]].y);
                cubeVertices.push_back(positions[indices[face][i]].z);
                
                // 法线
                cubeVertices.push_back(nm.x);
                cubeVertices.push_back(nm.y);
                cubeVertices.push_back(nm.z);
                
                // 纹理坐标
                cubeVertices.push_back(uvs[i].x);
                cubeVertices.push_back(uvs[i].y);
                
                // 切线
                cubeVertices.push_back(tangent.x);
                cubeVertices.push_back(tangent.y);
                cubeVertices.push_back(tangent.z);
                
                // 副切线
                cubeVertices.push_back(bitangent.x);
                cubeVertices.push_back(bitangent.y);
                cubeVertices.push_back(bitangent.z);
            }
        }

        // 设置立方体VAO/VBO
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), &cubeVertices[0], GL_STATIC_DRAW);
        
        // 顶点属性指针
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    
    // 绘制立方体
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36); // 6面×6顶点=36
    glBindVertexArray(0);
}





#pragma region 窗口事件及图像处理模块
/// @brief 
/// @param path 图片路径
/// @param reverse 判断是否延x轴翻转
/// @return 纹理对象的ID
unsigned int loadTexture(const char*  path,bool reverse)
{
    unsigned int textureID;
    glGenTextures(1,&textureID);
    int width,height,nrComponents;
    stbi_set_flip_vertically_on_load(reverse); 
    unsigned char* data = stbi_load(path,&width,&height,&nrComponents,0);
    if(data)
    {
        GLenum format;
        if(nrComponents == 1)
            format = GL_RED;
        else if(nrComponents == 3)
            format = GL_RGB;
        else if(nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D,textureID);
        glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);//生成多级渐近纹理

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    //此处可以看出纹理对象开辟的内存（glGenTextures）实在堆区开辟的，这个id是在栈区的对象，被return之后就会被销毁，但是这块是传值出去的，所以不要紧
    return textureID;
}



#pragma endregion