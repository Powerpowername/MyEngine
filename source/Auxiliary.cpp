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

Camera::Camera()
{
    name += "Camera";

    transform = gameObject->transform();//GameObject在他继承得MonoBehavior中
	transform->Pitch = radians(15.0f);
	transform->Yaw = radians(180.0f);
	viewPort = vec4(0, 0, pWindowSize->x, pWindowSize->y);//暂时不修改，看看后期有没有优化的办法，放到Setting里是不是更好
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

CameraMove::CameraMove()
{
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
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void CameraMove::Update()
{
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
}
#pragma endregion

#pragma region Transform : MonoBehavior
void Transform::Translate(vec3 movement)
{
    position += movement;
}

mat4 Transform::GetModelMaterix(mat4 world = mat4(1.0f)) const
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
Transform::Transform(vec3 pos, vec3 rotation, vec3 scanle) :
	position(pos),
	rotation(rotation),
	scale(scanle)
{
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
#pragma region