#include"base.h"
#include"Save.h"

#pragma region 函数声明
// 加载立方体贴图（6个面的纹理路径）
unsigned int loadCubemap(vector<std::string> faces);
// 键盘输入处理回调
void KeyBoardCallBack(GLFWwindow* window);
// 鼠标移动处理回调
void MouseCallBack(GLFWwindow* window, double xPos, double yPos);
// 窗口尺寸变化回调
void OnSize(GLFWwindow* window, int width, int height);
// GLFW错误回调（打印错误信息）
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
// 带目录路径的纹理加载（支持gamma校正）
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);
// 核心纹理加载方法（指定内部格式和纹理单元）
unsigned int LoadIamgeToGPU(const char* filename, GLint internalFormat, GLenum format, int textureSlot);
#pragma endregion
// unsigned int loadTexture(const char*  path,bool reverse=false);
int main(int argc, char* argv[])
{
	Setting::workDir = argv[0];  // 获取可执行文件路径作为工作目录

	/* 窗口创建和OpenGL初始化 */
	#pragma region Open a window
	// 设置错误回调函数
	glfwSetErrorCallback(glfw_error_callback);

	// 初始化GLFW库
	if (!glfwInit())
		return 1;
	
	// 配置OpenGL上下文版本（3.0）
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	// 创建窗口对象（尺寸来自Setting::windowSize）
	Setting::window = glfwCreateWindow(Setting::pWindowSize.x, Setting::pWindowSize.y, "nothing here", NULL, NULL);
	if (Setting::window == NULL)
		return 1;
	
	// 设置当前线程的OpenGL上下文
	glfwMakeContextCurrent(Setting::window);
	// 启用垂直同步（1=启用）
	glfwSwapInterval(1);
	
	// 初始化GLAD加载器（获取OpenGL函数指针）
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	#pragma endregion

	/* ImGui初始化 */
	#pragma region Init_IMGUI
	IMGUI_CHECKVERSION();              // 检查ImGui版本兼容性
	ImGui::CreateContext();            // 创建ImGui上下文
	ImGuiIO& io = ImGui::GetIO();      // 获取IO对象引用
	(void)io;                          // 避免未使用警告
	ImGui::StyleColorsDark();          // 设置暗色主题
	ImGui_ImplGlfw_InitForOpenGL(Setting::window, true);  // 初始化GLFW绑定
	ImGui_ImplOpenGL3_Init(glsl_version);       // 初始化OpenGL3绑定
	#pragma endregion

	/* OpenGL基础设置 */
	#pragma region Init_OpenGL
	// 设置鼠标移动回调
	// glfwSetCursorPosCallback(window, MouseCallBack);
	// 设置窗口尺寸变化回调
	glfwSetFramebufferSizeCallback(Setting::window, OnSize);
	// 初始视口设置（左下角原点，全窗口尺寸）
	glViewport(0, 0, Setting::pWindowSize.x, Setting::pWindowSize.y);
	// 启用深度测试（3D空间遮挡关系）
	glEnable(GL_DEPTH_TEST);
	#pragma endregion

	/* 游戏模块初始化 */
	#pragma region Init_Modules
	// Setting::InitSettings();  // 初始化游戏设置（窗口尺寸等）
	Input::InitInput();       // 初始化输入系统（按键状态数组）
	#pragma endregion

	// 截图工具实例化
	ScreenshotMaker sm;
	// 当前选中游戏对象指针（用于编辑器交互）
	static Object* selected = nullptr;
	Shader shader("shader_","E:\\GitStore\\MyEngine\\MyEngine\\shaderSource\\box/box");
	shader.setInt("material.texture_diffuse0",0);
	shader.setInt("material.texture_normal0",1);
	shader.setFloat("material.shininess",0.03);
	glm::mat4 model = glm::mat4(1.0f);
	//光源
	DirctionLight dirlight1(vec3(1,1,1),vec3(0.5f,1.0f,0.3));
	dirlight1.transform->Forward = vec3(0,0,1);
	// std::cout<<dirlight1.showID();
	static GLuint quadVAO = 0;
	static GLuint quadVBO = 0;
	std::list<Object*> cameras;
	Camera MainCamera("MainCamera");
	cameras.push_back(&MainCamera);
	Setting::GloabContorl.push_back(cameras);
	unsigned int diffuseMap = loadTexture("E:\\LearnOpenGL-master\\resources\\textures\\bricks2.jpg",false);
	unsigned int normalMap = loadTexture("E:\\LearnOpenGL-master\\resources\\textures\\bricks2_normal.jpg", false);
	Setting::MainCamera = &MainCamera;
	Setting::MainCamera->Update();
	static float lastTime = 0;
	static float currentTime = 0;
	while (!glfwWindowShouldClose(Setting::window))
	{
		// 用于计算帧间隔的时间戳
		currentTime = glfwGetTime();

		Setting::deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		/* 帧开始准备 */
#pragma region Others
		// 清屏颜色设置（ImGui可调参数）
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 处理键盘输入状态
		KeyBoardCallBack(Setting::window);
#pragma endregion

		/* ImGui帧初始化 */
#pragma region Init_ImGui
		glfwPollEvents();                     // 处理窗口事件
		ImGui_ImplOpenGL3_NewFrame();         // 开始新ImGui帧
		ImGui_ImplGlfw_NewFrame();            // 处理ImGui输入
		ImGui::NewFrame();                    // 创建新帧
#pragma endregion
		ImGui::Begin("MyEngine");
		if (ImGui::BeginMenu("GameObject"))
		{

			ImGui::EndMenu();
		}

		// ImGui::BeginMenu("GameObject");
		// {
			if(ImGui::Button("addCamera"))
			{
				// new GameObject("NewCamera", GameObject::GameObject_Camera);
			}
			Setting::MainCamera->OnGUI();

		ImGui::End();

		ImGui::Begin("Light");
		// ImGui::BeginMenu("GameObject");
		// {
			if(ImGui::BeginMenu("ShowLight"))
			{
				ImGui::EndMenu();
			}

			dirlight1.OnGUI();

			
			
		ImGui::End();


		// if(Setting::MainCamera != nullptr)
		// {
		// 	//此处后期多添加一个逻辑，处理是不是相关脚本的逻辑，这里就暂时默认只有camera脚本
		// 	for(auto it : *Setting::MainCamera->scripts)
		// 	{
		// 		// std::cout<<"------------"<<typeid(*it).name()<<std::endl;
		// 		if(typeid(*it) == typeid(Camera))
		// 		{
		// 			// std::cout<<"------------"<<typeid(*it).name()<<std::endl;
		// 			shader.setMat4("projection",static_cast<Camera*>(it)->projMat);
		// 			shader.setMat4("view",static_cast<Camera*>(it)->viewMat);
		// 			//model矩阵可以单独拿出来做一个放大缩小的处理
		// 		}
		// 	}
		// }

		mat4 model = mat4(1);
		shader.use();
		shader.setVec3("viewPos", Setting::MainCamera->transform->position);
		shader.setMat4("projection",Setting::MainCamera->projMat);
		shader.setMat4("view",Setting::MainCamera->viewMat);
		shader.setMat4("model",model);
		// shader.setBool("DirctionLight[0].flag",1);
		// shader.setVec3("DirctionLight[0].pos", dirlight1.transform->position);
		// shader.setVec3("DirctionLight[0].color", dirlight1.LightColor);
		// shader.setVec3("DirctionLight[0].dirToLight", dirlight1.transform->Forward);
		dirlight1.setShader(shader);
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
		RenderBox(shader,quadVAO,quadVBO);



			Input::GetInput(); // 更新输入状态

		// 遍历更新所有组件
		// for (auto go : *Setting::gameObjects)
		// 	if (go->enable)
		// 		for (auto mb : *(go->scripts))
		// 			if (mb->enable)
		// 				mb->Update(); // 常规更新逻辑
		Setting::MainCamera->Update();
		Input::ClearInputEveryFrame(); // 清空本帧输入
		lastTime = glfwGetTime();	   // 更新时间戳

		// Setting::MainCamera->Update();
		/* ImGui渲染 */
		#pragma region RenderImGui
		ImGui::Render();  // 生成绘制数据
		int display_w, display_h;
		glfwGetFramebufferSize(Setting::window, &display_w, &display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // OpenGL渲染
		glfwSwapBuffers(Setting::window); // 交换前后缓冲区
		#pragma endregion
	}

	/* 资源清理 */
	#pragma region Clear_ImGui
	ImGui_ImplOpenGL3_Shutdown();  // 关闭OpenGL渲染
	ImGui_ImplGlfw_Shutdown();     // 关闭GLFW绑定
	ImGui::DestroyContext();       // 销毁ImGui上下文
	#pragma endregion

	// 销毁窗口并终止GLFW
	glfwDestroyWindow(Setting::window);
	glfwTerminate();
	return 0;
}

/* 回调函数实现 */
#pragma region CallBack
// 键盘输入处理
void KeyBoardCallBack(GLFWwindow* window)
{
	static bool laststate = false;

	/* 移动控制 */
	#pragma region Move
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true); // ESC退出
	
	// WASDQE按键状态记录
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Input::Key[W_] = true;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Input::Key[S_] = true;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Input::Key[A_] = true;
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Input::Key[D_] = true;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) Input::Key[Q_] = true;
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) Input::Key[E_] = true;
	#pragma endregion

	/* 加速控制 */
	#pragma region Acceselate
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		Input::Key[Shift_] = true; // Shift加速
	#pragma endregion

	/* 鼠标锁定控制 */
	#pragma region LockMouse
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Input::Key[Space_] = true;
		// 原注释掉的鼠标锁定逻辑
	}
	#pragma endregion

	/* 调试功能 */
	#pragma region Debug
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		std::cout << glfwGetTime() << std::endl; // 打印当前时间
	#pragma endregion
}

// 鼠标移动处理
void MouseCallBack(GLFWwindow* window, double xPos, double yPos)
{
	static double lastX, lastY;
	static bool firstCome = true;

	// 初始化起始位置
	if (firstCome) {
		lastX = xPos;
		lastY = yPos;
		firstCome = false;
	}

	// 计算增量
	auto deltaX = xPos - lastX;
	auto deltaY = yPos - lastY;
	lastX = xPos;
	lastY = yPos;

	Input::mouseMentDelta = vec2(deltaX, deltaY); // 存储鼠标移动量
}

// 窗口尺寸变化处理
void OnSize(GLFWwindow* window, int width, int height)
{
	Setting::pWindowSize = vec2(width, height); // 更新窗口尺寸
	
	// 重新计算投影矩阵（60度FOV，0.1-100可视范围）
	// Setting::projMat = glm::perspective(radians(60.0f), (float)width/height, 0.1f, 100.0f);
	glViewport(0, 0, width, height); // 重置视口
}
#pragma endregion

/* 工具函数实现 */
#pragma region Functions


// 带目录路径的纹理加载
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = directory + '/' + string(path); // 拼接完整路径
	unsigned int textureID;
	glGenTextures(1, &textureID); // 生成纹理对象

	// 加载图像数据（与上一个函数逻辑类似）
	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		// 确定格式并绑定纹理
		GLenum format;
		if (nrComponents == 1) format = GL_RED;
		else if (nrComponents == 3) format = GL_RGB;
		else if (nrComponents == 4) format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 设置相同的纹理参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture at: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

// 核心纹理加载方法（指定纹理单元）
unsigned int LoadIamgeToGPU(const char* filename, GLint internalFormat, GLenum format, int textureSlot)
{
	unsigned int tex1;
	glGenTextures(1, &tex1); // 生成纹理对象
	
	// 激活指定纹理单元
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, tex1); // 绑定为2D纹理

	stbi_set_flip_vertically_on_load(true); // 垂直翻转图像

	int weight, height, nrVhannel;
	unsigned char* data = stbi_load(filename, &weight, &height, &nrVhannel, 0);
	if (data)
	{
		// 上传纹理数据
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, weight, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // 生成mipmap
	}
	else
	{
		printf("no image"); // 加载失败提示
	}
	stbi_image_free(data); // 释放图像内存
	return tex1;
}

// 加载立方体贴图（天空盒等）
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID); // 生成纹理对象
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // 绑定为立方体贴图

	int width, height, nrChannels;
	// 遍历6个面
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			// 依次设置每个面的纹理数据
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load cubemap texture: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	// 设置立方体贴图参数
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
#pragma endregion

