#include"base.h"
#include"Save.h"
static void glfw_error_callback(int error, const char* description);
void KeyBoardCallBack(GLFWwindow* window);
void MouseCallBack(GLFWwindow* window, double xPos, double yPos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void renderQuad();
static GLuint quadVAO = 0;
static GLuint quadVBO = 0;
int main(int argc, char* argv[])
{
	Setting::workDir = argv[0];  // 获取可执行文件路径作为工作目录

	/* 窗口创建和OpenGL初始化 */
	#pragma region Open a window
	// 设置错误回调函数
	glfwSetErrorCallback(glfw_error_callback);

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
	glfwSetFramebufferSizeCallback(Setting::window, framebuffer_size_callback);
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
	Shader shader("shader_","E:/GitStore/MyEngine/MyEngine/shaderSource/box/box");

	//配置shader的图像资源
	unsigned int diffuseMap = loadTexture("E:\\LearnOpenGL-master\\resources\\textures\\bricks2.jpg",false);
    unsigned int normalMap = loadTexture("E:\\LearnOpenGL-master\\resources\\textures\\bricks2_normal.jpg",false);
	shader.use();
	shader.setInt("material.texture_diffuse0",0);
	shader.setInt("material.texture_normal0",1);
	glm::vec3 lightPos(0.0f,1.0f,0.3f);

	//配置光源数据
	shader.setBool("lightDirectional[0].flag",1);
	shader.setVec3("lightDirectional[0].color",vec3(0,0,0));
	shader.setVec3("lightDirectional[0].pos",lightPos);
	shader.setVec3("lightDirectional[0].dirToLight",vec3(0,0,-1));
	shader.setFloat("material.shininess", 32.0f);
	//放入主摄像机
	std::list<Object*> cameras;
	Camera MainCamera("MainCamera");
	MainCamera.viewPort = vec4(0,0,Setting::pWindowSize);
	cameras.push_back(&MainCamera);

	Setting::GloabContorl.push_back(cameras);
	Setting::MainCamera = static_cast<Camera*>(*(cameras.begin()));//此处(*(cameras.begin()))要加括号，因为static_cast的优先级高于*

	while (!glfwWindowShouldClose(Setting::window))
	{

		static float lastTime = 0;
		// 清屏颜色设置（ImGui可调参数）
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 处理键盘输入状态
		KeyBoardCallBack(Setting::window);


		/* ImGui帧初始化 */

		glfwPollEvents();                     // 处理窗口事件
		ImGui_ImplOpenGL3_NewFrame();         // 开始新ImGui帧
		ImGui_ImplGlfw_NewFrame();            // 处理ImGui输入
		ImGui::NewFrame();                    // 创建新帧

		ImGui::Begin("MyEngine");

		if (ImGui::BeginMenu("GameObject"))
		{

			ImGui::EndMenu();
		}

		// ImGui::BeginMenu("GameObject");
		// {
			if(ImGui::Button("camera"))
			{
				// new GameObject("NewCamera", GameObject::GameObject_Camera);
			}
				Setting::MainCamera->OnGUI();

		// }
		// ImGui::EndMenu();
		// for(auto cameraTemp : Setting::gameObjects[GameObject::GameObject_Camera])
		// {
		// 	for(auto cameraTempScript : *(cameraTemp->scripts))
		// 	{
		// 		cameraTempScript->OnGUI();
		// 		cameraTempScript->Update();
		// 	}
		// }
		

		ImGui::End();

		shader.use();

		if(Setting::MainCamera != nullptr)
		{
			shader.setVec3("viewPos",Setting::MainCamera->transform->position);
			shader.setMat4("view",Setting::MainCamera->viewMat);
			shader.setMat4("projection",Setting::MainCamera->projMat);
		}
		glm::mat4 model = glm::mat4(1);
		shader.setMat4("model",model);

		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
		RenderBox(shader,quadVAO,quadVBO);



		// /* 游戏逻辑更新 */
		// #pragma region MainLoop_Update
		if (glfwGetTime() - lastTime >= Setting::deltaTime)
		{
			Input::GetInput();  // 更新输入状态
			
			// 遍历更新所有组件
			// for (auto go : *Setting::gameObjects)
			// 	if (go->enable)
			// 		for (auto mb : *(go->scripts))
			// 			if (mb->enable)
			// 				mb->Update(); // 常规更新逻辑
			Setting::MainCamera->Update();
			Input::ClearInputEveryFrame(); // 清空本帧输入
			lastTime = glfwGetTime();      // 更新时间戳
		}
		// #pragma endregion

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
// GLFW错误回调（打印错误信息）
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// 键盘输入处理
void KeyBoardCallBack(GLFWwindow* window)
{
	static bool laststate = false;

	/* 移动控制 */
	#pragma region Move
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true); // ESC退出
	
	// WASDQE按键状态记录
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		std::cout<<"1";
		Input::Key[W_] = true;
	}
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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Setting::pWindowSize = vec2(width, height); // 更新窗口尺寸
	
	// 重新计算投影矩阵（60度FOV，0.1-100可视范围）
	// Setting::projMat = glm::perspective(radians(60.0f), (float)width/height, 0.1f, 100.0f);
	glViewport(0, 0, width, height); // 重置视口
}
#pragma endregion

/* 工具函数实现 */
#pragma region Functions



#pragma endregion
