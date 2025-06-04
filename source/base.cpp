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

void Transform::OnGUI()
{
	ImGui::DragFloat3((Parent->name + "_position").c_str(), (float*)&position, 0.5f, -500, 500);
	ImGui::DragFloat3((Parent->name + "_rotation").c_str(), (float*)&rotation, 0.5f, -500, 500);
	ImGui::DragFloat3((Parent->name + "_scale").c_str(), (float*)&scale, 0.01f, -100, 100);
	ImGui::DragFloat((Parent->name + "_Yaw").c_str(), (float*)&Yaw, 0.01f, -180, 180);
	ImGui::DragFloat((Parent->name + "_Pitch").c_str(), (float*)&Pitch, 0.01f, -89, 89);
}

void Transform::Update()
{
	Forward.x = cos(Pitch)*sin(Yaw);
	Forward.y = sin(Pitch);
	Forward.z = cos(Pitch)*cos(Yaw);
    Forward = glm::normalize(Forward);
	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Forward, Right));
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
    glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);//渲染的窗口
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