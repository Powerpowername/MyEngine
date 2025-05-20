#include"Auxiliary.h"
#include"Save.h"

/* 全局变量声明区域 */
#pragma region GloalVar
vec2* pWindowSize;       // 窗口尺寸指针（vec2类型，x=宽度,y=高度）
GLFWwindow* window;      // GLFW窗口对象指针
mat4 modelMat;           // 模型变换矩阵（物体空间->世界空间）
mat4 viewMat;            // 视图矩阵（世界空间->相机空间）
mat4 projMat;            // 投影矩阵（相机空间->裁剪空间）
string workDir;          // 存储可执行文件的工作目录路径
#pragma endregion

/* 函数前置声明 */
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
// 从文件加载2D纹理
unsigned int TextureFromFile(string filename);
// 带目录路径的纹理加载（支持gamma校正）
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);
// 核心纹理加载方法（指定内部格式和纹理单元）
unsigned int LoadIamgeToGPU(const char* filename, GLint internalFormat, GLenum format, int textureSlot);
#pragma endregion

int main(int argc, char* argv[])
{
	workDir = argv[0];  // 获取可执行文件路径作为工作目录

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	// 创建窗口对象（尺寸来自Setting::windowSize）
	window = glfwCreateWindow(Setting::windowSize.x, Setting::windowSize.y, "nothing here", NULL, NULL);
	if (window == NULL)
		return 1;
	
	// 设置当前线程的OpenGL上下文
	glfwMakeContextCurrent(window);
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
	ImGui_ImplGlfw_InitForOpenGL(window, true);  // 初始化GLFW绑定
	ImGui_ImplOpenGL3_Init(glsl_version);       // 初始化OpenGL3绑定
	#pragma endregion

	/* OpenGL基础设置 */
	#pragma region Init_OpenGL
	// 设置鼠标移动回调
	// glfwSetCursorPosCallback(window, MouseCallBack);
	// 设置窗口尺寸变化回调
	glfwSetFramebufferSizeCallback(window, OnSize);
	// 初始视口设置（左下角原点，全窗口尺寸）
	glViewport(0, 0, Setting::windowSize.x, Setting::windowSize.y);
	// 启用深度测试（3D空间遮挡关系）
	glEnable(GL_DEPTH_TEST);
	#pragma endregion

	/* 游戏模块初始化 */
	#pragma region Init_Modules
	Setting::InitSettings();  // 初始化游戏设置（窗口尺寸等）
	Input::InitInput();       // 初始化输入系统（按键状态数组）
	#pragma endregion

	// 截图工具实例化
	ScreenshotMaker sm;
	// 当前选中游戏对象指针（用于编辑器交互）
	static GameObject* selected = nullptr;

	/* 主游戏循环 */
	while (!glfwWindowShouldClose(window))
	{
		// 用于计算帧间隔的时间戳
		static float lastTime = 0;

		/* 帧开始准备 */
		#pragma region Others
		// 清屏颜色设置（ImGui可调参数）
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 处理键盘输入状态
		KeyBoardCallBack(window);
		#pragma endregion

		/* ImGui帧初始化 */
		#pragma region Init_ImGui
		glfwPollEvents();                     // 处理窗口事件
		ImGui_ImplOpenGL3_NewFrame();         // 开始新ImGui帧
		ImGui_ImplGlfw_NewFrame();            // 处理ImGui输入
		ImGui::NewFrame();                    // 创建新帧
		#pragma endregion

		/* 多相机渲染逻辑 */
		#pragma region MultCameraRender
		vector<Camera*> cameras;
		// 遍历所有游戏对象收集有效相机
		for (auto go : *Setting::gameObjects)
			if (go->enable)
				for (auto mb : *(go->scripts))
					if (typeid(*mb) == typeid(Camera) && mb->enable)
						cameras.push_back((Camera*)mb);

		// 每个相机单独渲染场景
		for (auto camera : cameras)
		{
			camera->Update();  // 更新相机矩阵

			/* 组件实际更新（在相机更新后） */
			#pragma region MainLoop_RealUpdate
			for (auto go : *Setting::gameObjects)
				if (go->enable)
					for (auto mb : *(go->scripts))
						if (mb->enable)
							mb->RealUpdate(); // 执行需要相机参数的更新
			#pragma endregion
		}
		#pragma endregion

		/* ImGui界面绘制 */
		#pragma region DrawImGui
		ImGui::Begin("Inspector");  // 创建属性面板窗口

		/* 顶部菜单栏 */
		#pragma region Menu
		if (ImGui::BeginMenu("GameObject"))
		{
			// 删除选中对象逻辑
			if (ImGui::MenuItem("DeleteGameObject"))
				if (selected)
				{
					Setting::gameObjects->remove(selected);
					delete selected;
					selected = nullptr;
				}
			// 创建各类游戏对象
			if (ImGui::MenuItem("CreateEmpty")) 
				new GameObject("NewEmpty", GameObject::Empty);
			
			if (ImGui::MenuItem("CreateCamera", NULL, true))
				new GameObject("NewCamera", GameObject::Cameras);
			
			if (ImGui::MenuItem("CreateSkybox", NULL, true))
				new GameObject("NewSkyBox", GameObject::Box);
			
			// 光源创建子菜单
			if (ImGui::BeginMenu("Light")) {
				if (ImGui::MenuItem("CreateDirectional")) 
					new GameObject("NewDirectionalLight", GameObject::Directional);
				if (ImGui::MenuItem("CreatePoint")) 
					new GameObject("NewPointLight", GameObject::Point);
				if (ImGui::MenuItem("CreateSpot")) 
					new GameObject("NewSpotLight", GameObject::Spot);
				ImGui::EndMenu();
			}
			
			// 模型创建子菜单
			if (ImGui::BeginMenu("Model")) {
				if (ImGui::MenuItem("CreatePerson")) 
					new GameObject("NewPerson", GameObject::Model);
				if (ImGui::MenuItem("CreateHouse")) 
					new GameObject("NewHouse", "东方场景01.obj", "standrand");
				if (ImGui::MenuItem("CreateRobot")) 
					new GameObject("NewRobot", "source\\Robot.obj", "standrand");
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		// 组件添加菜单
		if (ImGui::BeginMenu("Component")) {
			if (ImGui::MenuItem("Camera"))
				if (selected) selected->AddComponentStart<Camera>();
			
			// 光源组件子菜单
			if (ImGui::BeginMenu("Light")) {
				if (ImGui::MenuItem("DirectionalLight")) 
					if (selected) selected->AddComponentStart<LightDirectional>();
				if (ImGui::MenuItem("PointLight")) 
					if (selected) selected->AddComponentStart<LightPoint>();
				if (ImGui::MenuItem("SpotLight")) 
					if (selected) selected->AddComponentStart<LightSpot>();
				ImGui::EndMenu();
			}
			
			if (ImGui::MenuItem("Rotate"))
				if (selected) selected->AddComponentStart<Rotate>();
			
			ImGui::EndMenu();
		}
		#pragma endregion

		/* 折叠面板区域 */
		#pragma region 折叠窗
		// 全局设置面板
		if (ImGui::CollapsingHeader("Global")) {
			ImGui::ColorEdit4("ClearColor", (float*)&clear_color); // 颜色选择控件
		}

		// 游戏对象列表
		#pragma region MainLoop_OnGUI
		static bool camein = false;
		if (ImGui::CollapsingHeader("GameObjects")) {
			for (auto go : *Setting::gameObjects) {
				// 生成带ID的树节点
				if (ImGui::TreeNode((go->name + std::to_string(go->id)).c_str())) {
					camein = true;
					selected = go;       // 设置当前选中对象
					go->OnGUI();         // 显示对象属性
					
					// 遍历组件显示属性
					for (auto mb : *(go->scripts)) {
						if (ImGui::CollapsingHeader(mb->name.c_str())) {
							mb->OnGUI(); // 显示组件属性
						}
					}
					ImGui::TreePop();    // 关闭树节点
					ImGui::Spacing();     // 添加间距
				}
			}
		}
		#pragma endregion	
		
		sm.Draw();     // 绘制截图工具界面
		ImGui::End();  // 结束属性面板
		#pragma endregion

		// 重置选中状态
		if (!camein) selected = nullptr;

		/* 游戏逻辑更新 */
		#pragma region MainLoop_Update
		if (glfwGetTime() - lastTime >= Setting::deltaTime)
		{
			Input::GetInput();  // 更新输入状态
			
			// 遍历更新所有组件
			for (auto go : *Setting::gameObjects)
				if (go->enable)
					for (auto mb : *(go->scripts))
						if (mb->enable)
							mb->Update(); // 常规更新逻辑
			
			Input::ClearInputEveryFrame(); // 清空本帧输入
			lastTime = glfwGetTime();      // 更新时间戳
		}
		#pragma endregion

		/* ImGui渲染 */
		#pragma region RenderImGui
		ImGui::Render();  // 生成绘制数据
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // OpenGL渲染
		glfwSwapBuffers(window); // 交换前后缓冲区
		#pragma endregion
	}

	/* 资源清理 */
	#pragma region Clear_ImGui
	ImGui_ImplOpenGL3_Shutdown();  // 关闭OpenGL渲染
	ImGui_ImplGlfw_Shutdown();     // 关闭GLFW绑定
	ImGui::DestroyContext();       // 销毁ImGui上下文
	#pragma endregion

	// 销毁窗口并终止GLFW
	glfwDestroyWindow(window);
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
		cout << glfwGetTime() << std::endl; // 打印当前时间
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
	Setting::windowSize = vec2(width, height); // 更新窗口尺寸
	
	// 重新计算投影矩阵（60度FOV，0.1-100可视范围）
	projMat = perspective(radians(60.0f), (float)width/height, 0.1f, 100.0f);
	glViewport(0, 0, width, height); // 重置视口
}
#pragma endregion

/* 工具函数实现 */
#pragma region Functions
// 从文件加载2D纹理
unsigned int TextureFromFile(string filename)
{
	unsigned int textureID;
	glGenTextures(1, &textureID); // 生成纹理对象

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		// 确定OpenGL格式
		GLenum format;
		if (nrComponents == 1) format = GL_RED;
		else if (nrComponents == 3) format = GL_RGB;
		else if (nrComponents == 4) format = GL_RGBA;

		// 绑定并设置纹理
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load " << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

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