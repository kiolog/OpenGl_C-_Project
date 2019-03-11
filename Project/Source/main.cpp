#include "../Externals/Include/Include.h"

#include "Snow.cpp"
#include "pot.cpp"
#include "Tree.cpp"
#include "house.cpp"
#include "bridge.cpp"
#include "utilities.cpp"
#include "terrain.cpp"
#include "SmallMap.cpp"
#include <ctime>
#include "../Externals/Include/MyInclude/Fluid.h"
#include "../Externals/Include/MyInclude/MeshRenderer.h"
#include "../Externals/Include/MyInclude/Light.h"
#include "../Externals/Include/MyInclude/FirstPersonCamera.h"
#include "../Externals/Include/MyInclude/SkyBox.h"
#include "../Externals/Include/MyInclude/Window.h"
#include "../Externals/Include/MyInclude/ClickDetector.h"
#include "../Externals/Include/MyInclude/GameObject.h"
#include "../Externals/Include/MyInclude/MyFbxMesh.h"
#include "../Externals/Include/MyInclude/PlayerController.h"
#include "../Externals/Include/MyInclude/KeyBoardController.h"
#include "../Externals/Include/MyInclude/MovementSimulator.h"
#include "../Externals/Include/MyInclude/BaseCamera.h"
#include "../Externals/Include/MyInclude/ThirdPersonCamera.h"
#include "../Externals/Include/MyInclude/CollisionEngine.h"
#include "../Externals/Include/MyInclude/AIController.h"
#include "../Externals/Include/MyInclude/Mesh.h"
#include "../Externals/Include/MyInclude/AnimalManager.h"
#include "../Externals/Include/MyInclude/MyLib.h"
#include "../Externals/Include/MyInclude/AnimalMenu.h"
#include <functional>
#include <typeinfo>
#define MENU_TIMER_START 91
#define MENU_TIMER_STOP 92
//--------Yuyi---------------//
House houses;
SnowFall snowfall;
Lowpolytree trees;
Pot pot;
Bridge bridge;
//--------Yuyi---------------//
struct {
	vec2 win_size = vec2(600, 600);
	float aspect;
	void updateAspect(float width, float height) {
		win_size = vec2(width, height);
		aspect = width / height;
	}
	float worldSize = 600;
	float worldVertexNum = 255;
}Config;

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std;

Mouse myMouse;
Keyboard myKeyboard;

SmallMap smallMap(1024);

TerrainProgram terrainProgram;
Camera myCamera;
Terrains terrain;
//--------Yuyi---------------//

//--------Yuyi---------------//





//---------------------------

MeshRenderer *m_MeshRenderer;
Fluid *m_Fluid;
vec3 m_v3LightPosition = vec3(10000, 10000, 10000);
Light *m_LightModel;
mat4 scale_bias_matrix;
mat4 shadow_sbpv_matrix;
BaseCamera *m_FirstPersonCamera;
BaseCamera *m_ThirdPersonCamera;
mat4 m_ViewMatrix;
mat4 m_ProjMatrix;
float m_fDeltaTime = 0;
float m_fBeforeTime = 0;
bool m_bMoveMouse = false;
vec2 m_ImageSize;
SkyBox *m_SkyBox;
Window *m_Window;
ClickDetector *m_ClickDetector;
MovementSimulator *m_MovementSimulator;

vector<GameObject*> m_ListGameObject;
vector<GameObject*> m_ListTreeMesh;
bool m_bEnableCreateObject = false;

AnimalManager *m_AnimalManager;
AnimalMenu *m_AnimalMenu;
EAnimalType m_SelectAnimalType = WOLF;

//All Position
vector<vec2> m_ListHousePosition = {
	vec2(-176.0f,-42.0f),
	vec2(-176.0f, -105.0f),
	vec2(-129.0f, -147.0f),
	vec2(-86.0f, -105.0f),
	vec2(-86.0f, -42.0f), 
};
vector<vec2> m_ListTreePosition = {
	vec2(-60, 123.2),
	vec2(-44.8, 152.8),
	vec2(-60, 184.9),
	vec2(-45.1, 215),

	vec2(-90.7, 124.9),
	vec2(-75, 152.9),
	vec2(-90.2, 185.2),
	vec2(-75.1, 215.1),

	vec2(-104.8, 153.1),
	vec2(-120.7, 184.9),
	vec2(-105, 215.1),

	vec2(-135, 152.9),
	vec2(-150.1, 184.9),
	vec2(-134.9, 215.1),

	vec2(-165, 150.9),
	vec2(-180.8, 185.1),
	vec2(-164.9, 205.1),
};
vector<vec2> m_ListPotPosition = {
	vec2(-103.0f, -51.0f),
	vec2(-103.0f, -32.0f),
	vec2(-103.0f, -110.0f),
	vec2(-103.0f, -95.0f),
	vec2(-121.0f, -134.0f),
	vec2(-136.0f, -134.0f),
	vec2(-160.0f, -110.0f),
	vec2(-160.0f, -95.0f),
	vec2(-160.0f, -51.0f),
	vec2(-160.0f, -33.0f),
};

//
mat4 m_BridgeModelMatrix;
mat4 m_BridgeInvMatrix;
vec3 m_v3BridgeSize = vec3(-23.758675f,29.813274f, 40);



//---------------------------
void InitCamera() {
	mat4 InitViewMatrix = mat4(
		vec4(0.665f, 0.470f, -0.581f, 0.000f),
		vec4(0.000f, 0.778f, 0.629f, 0.000f),
		vec4(0.747f, -0.418f, 0.517f, 0.000f),
		vec4(-7.668f, 0.514f, -39.385f, 1.000f));
	//m_FirstPersonCamera = new FirstPersonCamera(vec3(),30);
	m_FirstPersonCamera = new FirstPersonCamera(50.f, InitViewMatrix, vec3(18.011, -24.365, -26.309), 0.680000, -5.440004);
	m_FirstPersonCamera = new ThirdPersonCamera();
	//view_matrix = m_Camera->GetViewMatrix();
	//m_Camera->SetViewMatrix(lookAt(vec3(), vec3(-1, -1, 0), vec3(0, 1, 0)));
	//m_ViewMatrix = m_FirstPersonCamera->GetViewMatrix();
}
void InitFluid() {
	m_Fluid = new Fluid(100, 100, vec3(0, 0, 0),vec2(6));
	m_Fluid->SetShadowMapTextureID(m_LightModel->GetDepthTexture());
	m_Fluid->SetShadowMatrix(shadow_sbpv_matrix);
}
void InitLight() {
	m_LightModel = new Light(m_v3LightPosition, vec3(0, 0, 0));
	m_LightModel->BindShadowBuffer();
	scale_bias_matrix = translate(mat4(), vec3(0.5f, 0.5f, 0.5f));
	scale_bias_matrix = scale(scale_bias_matrix, vec3(0.5f, 0.5f, 0.5f));

	shadow_sbpv_matrix = scale_bias_matrix * m_LightModel->GetViewProjMatrix();
}
void InitMeshRenderer() {
	m_MeshRenderer = new MeshRenderer();
	m_MeshRenderer->SetShadowMapTextureID(m_LightModel->GetDepthTexture());
	m_MeshRenderer->SetShadowMatrix(shadow_sbpv_matrix);
}
void InitSkyBox() {
	vector<std::string> ListCubeMap
	{
		"../SkyBox/skybox/right.jpg",
		"../SkyBox/skybox/left.jpg",
		"../SkyBox/skybox/bottom.jpg",
		"../SkyBox/skybox/top.jpg",
		"../SkyBox/skybox/front.jpg",
		"../SkyBox/skybox/back.jpg"
	};
	m_SkyBox = new SkyBox(ListCubeMap, vec3(50000, -50000, 50000));
}
bool IsInsideBridge(vec3 _v3Position) {
	bool bReturnValue = false;
	//PrintMat4("Inv Matrix", m_BridgeInvMatrix);
	vec3 v3WorldPosition = (m_BridgeInvMatrix * vec4(_v3Position, 1)).xyz;
	//printf("Relative Position : (%f, %f)\n", v3WorldPosition.x, v3WorldPosition.z);
	float fHalfWidth = m_v3BridgeSize.z * 0.5f;
	if (v3WorldPosition.x >= m_v3BridgeSize.x && v3WorldPosition.x <= m_v3BridgeSize.y && v3WorldPosition.z >= -fHalfWidth && v3WorldPosition.z <= fHalfWidth) {
		bReturnValue = true;
	}
	return bReturnValue;
}
float GetBridgeHeight(float _fX,float _fZ) {
	vec3 v3WorldPosition = (m_BridgeInvMatrix * vec4(_fX, 0, _fZ, 1)).xyz;
	float fValue = (v3WorldPosition.x + -m_v3BridgeSize.x) / (-m_v3BridgeSize.x + m_v3BridgeSize.y);
	float fPoint0 = 11.135964f;
	float fPoint1 = 18.f;
	float fPoint2 = 3.674572f;
	//printf("Value : %f\n", fValue);
	float fReturnValue = pow((1 - fValue), 2) * fPoint0 + 2 * fValue * (1 - fValue) * fPoint1 + pow(fValue, 2) * fPoint2;
	return fReturnValue;
}
float GetHeight(float _fX, float _fZ, float _fPreY) {
	float fReturnValue = 0;
	bool bIsInsideBridge = IsInsideBridge(vec3(_fX, 0, _fZ));

	if (bIsInsideBridge) {
		fReturnValue = GetBridgeHeight(_fX, _fZ);
		//fReturnValue = terrain.GetTerrainHeight(_fX, _fZ);
		//printf("Height : %f\n", fReturnValue);
	}
	else {
		fReturnValue = terrain.GetTerrainHeight(_fX, _fZ);
	}
	//printf("Height : %f\n", fReturnValue);
	//printf("Is Inside Bridge : %d\n", bIsInsideBridge);
	//printf("Terrain Height : %f\n", terrain.GetTerrainHeight(_fX, _fZ));

	return fReturnValue;
}
GameObject *CreateGameObjectWithBoundingBox(vec2 _v2Position,vec3 _v3BoundingBoxPosition,vec3 _v3BoundingBoxSize,float _fYaw = 0,float _fPitch = -90) {
	GameObject *NewGameObject = new GameObject();
	NewGameObject->SetCollisionType(OBSTACLE);
	NewGameObject->SetBaseYawAndPitchAndRoll(0, radians(_fPitch));
	NewGameObject->AddBoundingBox(_v3BoundingBoxPosition, _v3BoundingBoxSize);
	NewGameObject->SetWorldPosition(vec3(_v2Position.x, terrain.GetTerrainHeight(_v2Position.x, _v2Position.y), _v2Position.y));
	NewGameObject->SetYaw(radians(_fYaw));
	NewGameObject->RefreshPositionAndYaw();
	NewGameObject->UpdateModelMatrix();
	m_MovementSimulator->AddGameObject(NewGameObject);
	
	return NewGameObject;
}
vec3 GetRandomTargetPosition(vec2 _v2Position, float _fRadius) {
	int iAngle = rand() % 360;
	mat4 RotationMatrix = rotate(mat4(), deg2rad(iAngle), vec3(0, 1, 0));
	vec3 v3Direction = mat3(RotationMatrix) * vec3(1, 0, 0);
	float fDistance = _fRadius * ((rand() % 100) / 100.f);
	vec3 v3ReturnVector = v3Direction * fDistance + vec3(_v2Position.x, 0, _v2Position.y);
	v3ReturnVector.y = terrain.GetTerrainHeight(v3ReturnVector.x, v3ReturnVector.y);
	return v3ReturnVector;
}
GameObject *CreateAnimal(EAnimalType _AnimalType,vec2 _v2BasePoint,float _fRadius) {
	vec3 v3WorldPosition = GetRandomTargetPosition(_v2BasePoint, _fRadius);
	GameObject *NewGameObject = m_AnimalManager->CreateAnimal(_AnimalType);
	NewGameObject->SetWorldPosition(v3WorldPosition);
	NewGameObject->RefreshPositionAndYaw();
	m_ListGameObject.push_back(NewGameObject);
	m_Fluid->AddGameObject(NewGameObject);
	return NewGameObject;
}
void InitMoveSimulator() {
	m_AnimalMenu = new AnimalMenu();

	m_AnimalManager = new AnimalManager();
	vec2 v2MapSize = vec2(terrain.GetSize() * 0.5f);
	m_MovementSimulator = new MovementSimulator(std::bind(GetHeight, placeholders::_1, placeholders::_2, placeholders::_3), v2MapSize);
	//m_MovementSimulator->SetGetTerrainHeight(std::bind(&Terrains::GetTerrainHeight, &terrain, placeholders::_1, placeholders::_2));


	//GameObject *NewGameObject = new MyFbxMesh("../Model/Walf/Walking.fbx");
	GameObject *NewGameObject = new GameObject(30);
	NewGameObject->SetBaseYawAndPitchAndRoll(radians(90.f), 0);
	NewGameObject->SetBaseTranslation(vec3(0, 0, 0));
	NewGameObject->SetBaseScale(vec3(5));
	NewGameObject->AddBoundingBox(vec3(0, 10, 0), vec3(5,20,5));
	NewGameObject->UpdateModelMatrix();
	m_ListGameObject.push_back(NewGameObject);
	PlayerController *NewPlayerController = new KeyBoardController({ 'w','s','a','d','q','e' });
	NewPlayerController->AddGameObject(NewGameObject);

	m_MovementSimulator->AddPlayerController(NewPlayerController);
	m_MovementSimulator->AddGameObject(NewGameObject);
	m_Fluid->AddGameObject(NewGameObject);
	dynamic_cast<ThirdPersonCamera*>(m_FirstPersonCamera)->SetFollowObject(NewGameObject, vec3(0, 10, 0));

	m_MovementSimulator->AddToAIController(CreateAnimal(COW, vec2(-50, -50), 200), vec2(-50, -50), 200);
	m_MovementSimulator->AddToAIController(CreateAnimal(COW, vec2(-50, -50), 200), vec2(-50, -50), 200);
	m_MovementSimulator->AddToAIController(CreateAnimal(COW, vec2(-50, -50), 200), vec2(-50, -50), 200);
	m_MovementSimulator->AddToAIController(CreateAnimal(COW, vec2(-50, -50), 200), vec2(-50, -50), 200);
	m_MovementSimulator->AddToAIController(CreateAnimal(COW, vec2(-50, -50), 200), vec2(-50, -50), 200);
	m_MovementSimulator->AddToAIController(CreateAnimal(COW, vec2(-50, -50), 200), vec2(-50, -50), 200);
	

	/*CreateHouse(vec2(-50, -70), 0);
	CreateHouse(vec2(-120, -70), 30);
	CreateHouse(vec2(-190, -70), 70);*/
	
	//NewGameObject = new MyFbxMesh("../Model/Windmill/Windmill.fbx");
	/*NewGameObject = new Mesh("../Assets/Bridge2.obj","../Assets/");
	NewGameObject->SetBaseScale(vec3(2));
	NewGameObject->SetBaseYawAndPitchAndRoll(radians(-90.f), radians(0.f), radians(0.f));
	NewGameObject->SetWorldPosition(vec3(0, 0, 0));
	NewGameObject->SetBaseTranslation(vec3(0, 10, 0));
	NewGameObject->UpdateModelMatrix();
	m_ListGameObject.push_back(NewGameObject);*/


	int iListCount = m_ListHousePosition.size();
	for (int i = 0; i < iListCount; ++i) {
		CreateGameObjectWithBoundingBox(m_ListHousePosition[i], vec3(0, 10, 0), vec3(20));
	}
	iListCount = m_ListTreePosition.size();
	for (int i = 0; i < iListCount; ++i) {
		CreateGameObjectWithBoundingBox(m_ListTreePosition[i], vec3(0, 10, 0), vec3(15, 20, 15));
	}
	iListCount = m_ListPotPosition.size();
	for (int i = 0; i < iListCount; ++i) {
		CreateGameObjectWithBoundingBox(m_ListPotPosition[i], vec3(0, 3, 0), vec3(2));
	}
	m_BridgeModelMatrix = translate(mat4(), vec3(-98.0f, 0.0f, 63.0f)) * rotate(mat4(), radians(120.f), vec3(0, 1, 0));
	m_BridgeInvMatrix = inverse(m_BridgeModelMatrix);


	CreateGameObjectWithBoundingBox(vec2(-98.0f, 63.0f), vec3(5, 10, 17.5f), vec3(54, 50, 5),120,0);
	CreateGameObjectWithBoundingBox(vec2(-98.0f, 63.0f), vec3(5, 10, -17.5f), vec3(54, 50, 5),120,0);
}
void My_Init(){


	printf("GL_DRAW_FRAMEBUFFER = %x\n", GL_DRAW_FRAMEBUFFER);
	printf("GL_FRAMEBUFFER = %x\n", GL_FRAMEBUFFER);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	myCamera.initialize(Config.win_size);
	terrainProgram.initialize();
	terrain.initilaze(Config.worldVertexNum, Config.worldSize);
	
	//smallMap.initialize();
	//smallMap.generateTexture(terrain, terrainProgram, Config.win_size);

	//----Snow program------------//
	snowfall.InitSnowProgram();
	houses.InitHouseprogram();
	trees.InitTreeprogram();
	pot.InitPotprogram();
	bridge.InitBridgeprogram();
	//---house---//
	


	//------------
	InitCamera();
	InitLight();
	InitFluid();
	InitSkyBox();
	InitMoveSimulator();
	InitMeshRenderer();
	m_Window = new Window();
	m_ClickDetector = new ClickDetector();
	m_ClickDetector->SetTerrainSize(terrain.GetSize());
	/*std::function<float(float, float)> Temp;
	auto TestFunction = std::bind(&Terrains::GetTerrainHeight, &terrain, placeholders::_1, placeholders::_2);
	Temp = TestFunction;
	printf("Height : %f\n", terrain.GetTerrainHeight(50, 50));
	printf("Height : %f\n", Temp(50, 50));

	PlayerController *NewController = new PlayerController();
	NewController->SetGetTerrainHeight(TestFunction);
	NewController->PrintHeight(50, 50);


	//Test---------------
	BoundingBox SourceBoundingBox(vec3(0, 0, 0), vec3(10, 10, 10));
	BoundingBox TargetBoundingBox(vec3(10.1f, 0, 0), vec3(10, 10, 10));

	bool bIsOverlap = IsOverlap(vec3(0, 0, 0), SourceBoundingBox, vec3(0), TargetBoundingBox);
	printf("Is Overlap : %d\n", bIsOverlap);*/

	
}
void UpdateDeltaTime() {
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	m_fDeltaTime = (currentTime - m_fBeforeTime) * 0.001;
	m_fBeforeTime = currentTime;
	//printf("DeltaTime : %f\n", m_fDeltaTime);
}
void UpdateCamera() {
	m_FirstPersonCamera->UpdateView(m_fDeltaTime);
	m_ViewMatrix = m_FirstPersonCamera->GetViewMatrix();
}
void UpdateFluid() {
	m_Fluid->SetEyePosition(m_FirstPersonCamera->GetEyePosition());
	m_Fluid->SetViewMatrix(m_FirstPersonCamera->GetViewMatrix());
	m_Fluid->SetProjMatrix(m_ProjMatrix);
	m_Fluid->SetViewingDirection(m_FirstPersonCamera->GetForwardDirection());
	m_Fluid->SetLightPosition(m_LightModel->GetLightPosition());
	m_Fluid->UpdateCollision();
}
void ClearColor() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0f);
}
void DrawSkyBox(mat4 _Matrix) {
	m_SkyBox->SetViewMatrix(_Matrix);
	m_SkyBox->SetProjMatrix(m_ProjMatrix);
	m_SkyBox->Render();
}

void UpdateObjectState() {
	vector<GameObject*> ListAnimal = m_AnimalManager->GetAllObject();
	int iListCount = ListAnimal.size();
	for (int i = 0; i < iListCount; ++i) {
		GameObject *TraceGameObject = ListAnimal[i];
		TraceGameObject->UpdateLifeTime(m_fDeltaTime);

		if (TraceGameObject->GetObjectState() == DIE) {
			printf("Object Is Die!!!\n");
			RemoveObjectFromVector(m_ListGameObject, TraceGameObject);
			m_MovementSimulator->RemoveObject(TraceGameObject);
			m_Fluid->RemoveObject(TraceGameObject);
			m_AnimalManager->RemoveObject(TraceGameObject);
		}
	}
}
void UpdateMovement() {
	m_MovementSimulator->Update(m_fDeltaTime);
}
void DrawShadowMap() {
	m_LightModel->ShadowMapSetting();
	//m_MeshRenderer->SetModelMatrix(m_FBXMesh->GetModelMatrix());
	m_MeshRenderer->SetViewMatrix(m_LightModel->GetViewMatrix());
	m_MeshRenderer->SetProjMatrix(m_LightModel->GetProjMatrix());
	m_MeshRenderer->SetLightPosition(m_LightModel->GetLightPosition());
	m_MeshRenderer->SetEyePosition(m_FirstPersonCamera->GetEyePosition());
	m_MeshRenderer->Setting(vec4(0, 0, 0, 1));
	int iListCount = m_ListGameObject.size();
	//printf("GameObject Number : %d\n", iListCount);
	for (int i = 0; i < iListCount; ++i) {
		GameObject *TraceGameObject = m_ListGameObject[i];
		m_MeshRenderer->SetModelMatrix(TraceGameObject->GetModelMatrix());
		m_MeshRenderer->Setting(vec4(0, 0, 0, 1));
		//PrintMat4("Model Matrix", TraceGameObject->GetModelMatrix());
		TraceGameObject->Render();
	}
	//terrain.render(m_FirstPersonCamera->GetFlipViewMatrix(m_Fluid->GetPlane()), m_ProjMatrix, m_LightModel->GetLightPosition(), m_Fluid->GetPlane(), terrainProgram);
	//terrain.render(m_LightModel->GetViewMatrix(), m_LightModel->GetProjMatrix(), m_LightModel->GetLightPosition(), vec4(0, 0, 0, 1), shadow_sbpv_matrix, m_LightModel->GetDepthTexture(), terrainProgram);
	//DrawSkyBox(m_FirstPersonCamera->GetViewMatrix());
	m_Fluid->Render();
}
void RenderAllGameObject() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_ImageSize[0], m_ImageSize[1]);
	//m_MeshRenderer->SetModelMatrix(m_FBXMesh->GetModelMatrix());
	m_MeshRenderer->SetViewMatrix(m_FirstPersonCamera->GetViewMatrix());
	m_MeshRenderer->SetProjMatrix(m_ProjMatrix);
	m_MeshRenderer->SetLightPosition(m_LightModel->GetLightPosition());
	m_MeshRenderer->SetEyePosition(m_FirstPersonCamera->GetEyePosition());
	m_MeshRenderer->Setting(vec4(0, 0, 0, 1));
	int iListCount = m_ListGameObject.size();
	//printf("GameObject Number : %d\n", iListCount);
	for (int i = 0; i < iListCount; ++i) {
		GameObject *TraceGameObject = m_ListGameObject[i];
		m_MeshRenderer->SetModelMatrix(TraceGameObject->GetModelMatrix());
		m_MeshRenderer->Setting(vec4(0, 0, 0, 1));
		//PrintMat4("Model Matrix", TraceGameObject->GetModelMatrix());
		TraceGameObject->Render();
	}
	//glEnable(GL_CLIP_DISTANCE0);
	//terrain.render(m_FirstPersonCamera->GetFlipViewMatrix(m_Fluid->GetPlane()), m_ProjMatrix, m_LightModel->GetLightPosition(), m_Fluid->GetPlane(), terrainProgram);
	terrain.render(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix, m_LightModel->GetLightPosition(), vec4(0,0,0,1),shadow_sbpv_matrix,m_LightModel->GetDepthTexture(), terrainProgram);
	DrawSkyBox(m_FirstPersonCamera->GetViewMatrix());
	houses.DrawHouse(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	snowfall.setCamera(m_FirstPersonCamera->GetEyePosition());
	snowfall.DrawParticle(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	trees.DrawTree(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	pot.DrawPot(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	bridge.DrawBridge(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	m_Fluid->Render();

	
}
void UpdateClickDetector() {
	m_ClickDetector->InitSetting();
	m_ClickDetector->SettingMatrix(terrain.getModelMatrix(), m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	terrain.render();
	m_ClickDetector->SettingMatrix(m_BridgeModelMatrix * bridge.GetModelMatrix(), m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	bridge.DrawBridge();
}
void DrawReflectionAndRefraction() {
	m_Fluid->ReflectionSetting();
	terrain.render(m_FirstPersonCamera->GetFlipViewMatrix(m_Fluid->GetPlane()), m_ProjMatrix, m_LightModel->GetLightPosition(), m_Fluid->GetPlane(), shadow_sbpv_matrix, m_LightModel->GetDepthTexture(), terrainProgram);
	//terrain.render(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix, m_LightModel->GetLightPosition(), m_Fluid->GetRefractionPlane(), terrainProgram);
	//ClearColor();
	DrawSkyBox(m_FirstPersonCamera->GetFlipViewMatrix(m_Fluid->GetPlane()));
	m_MeshRenderer->SetViewMatrix(m_FirstPersonCamera->GetFlipViewMatrix(m_Fluid->GetPlane()));
	m_MeshRenderer->SetProjMatrix(m_ProjMatrix);
	m_MeshRenderer->SetLightPosition(m_LightModel->GetLightPosition());
	m_MeshRenderer->SetEyePosition(m_FirstPersonCamera->GetEyePosition());
	m_MeshRenderer->Setting(vec4(0, 0, 0, 1));
	int iListCount = m_ListGameObject.size();
	//printf("GameObject Number : %d\n", iListCount);
	for (int i = 0; i < iListCount; ++i) {
		GameObject *TraceGameObject = m_ListGameObject[i];
		m_MeshRenderer->SetModelMatrix(TraceGameObject->GetModelMatrix());
		m_MeshRenderer->Setting(vec4(0, 0, 0, 1));
		//PrintMat4("Model Matrix", TraceGameObject->GetModelMatrix());
		TraceGameObject->Render();
	}

	m_Fluid->RefractionSetting();
	terrain.render(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix, m_LightModel->GetLightPosition(), vec4(0,0,0,1), shadow_sbpv_matrix, m_LightModel->GetDepthTexture(), terrainProgram);

	m_MeshRenderer->SetViewMatrix(m_FirstPersonCamera->GetViewMatrix());
	m_MeshRenderer->SetProjMatrix(m_ProjMatrix);
	m_MeshRenderer->SetLightPosition(m_LightModel->GetLightPosition());
	m_MeshRenderer->SetEyePosition(m_FirstPersonCamera->GetEyePosition());
	m_MeshRenderer->Setting(vec4(0, 0, 0, 1));
	iListCount = m_ListGameObject.size();
	//printf("GameObject Number : %d\n", iListCount);
	for (int i = 0; i < iListCount; ++i) {
		GameObject *TraceGameObject = m_ListGameObject[i];
		m_MeshRenderer->SetModelMatrix(TraceGameObject->GetModelMatrix());
		m_MeshRenderer->Setting(m_Fluid->GetRefractionPlane());
		//PrintMat4("Model Matrix", TraceGameObject->GetModelMatrix());
		TraceGameObject->Render();
	}
}
void My_Display(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	myCamera.updateSmooth();
	myKeyboard.updateCounter();

	//----------
	UpdateDeltaTime();
	UpdateCamera();
	UpdateObjectState();
	UpdateMovement();
	UpdateFluid();
	UpdateClickDetector();
	//DrawShadowMap();
	DrawReflectionAndRefraction();
	RenderAllGameObject();
	m_AnimalMenu->Render();
	//UpdateClickDetector();
	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	m_Window->SetTextureID(m_ClickDetector->GetTextureID());
	m_Window->Render();*/
	mat4 terrainModelMatrix = terrain.getModelMatrix();
	//terrain.setModelMatrix(rotate(terrainModelMatrix, deg2rad(1), vec3(0,1,0)));
	//terrain.render(myCamera, terrainProgram);
	//PrintMat4("ViewMatrix", m_FirstPersonCamera->GetViewMatrix());
	//terrain.render(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix, m_LightModel->GetLightPosition(), terrainProgram);
	//------Yuyi------------//
	//houses.DrawHouse(myCamera);
	//trees.DrawTree(myCamera);
	//snowfall.setCamera(myCamera.getPosition());
	//snowfall.setCamera(m_FirstPersonCamera->GetEyePosition());
	//snowfall.DrawParticle(m_FirstPersonCamera->GetViewMatrix(), m_ProjMatrix);
	//snowfall.DrawParticle(myCamera.getViewMatrix(), myCamera.getProjectionMatrix());
	
	//------Yuyi------------//
	//smallMap.render();
	
	//m_Terrain->Render(vec4(0, 0, 0, 1));
	//DrawReflectionAndRefraction();
	
	glutSwapBuffers();
}

void My_Reshape(int width, int height){
	glViewport(0, 0, width, height);
	float viewportAspect = (float)width / (float)height;
	Config.updateAspect((float)width, (float)height);
	myCamera.updateProj(Config.win_size);
	m_ProjMatrix = perspective(deg2rad(80.0f), viewportAspect, 0.1f, 100000.0f);
	m_Fluid->Reshape(width, height);
	m_ImageSize = vec2(width, height);
	m_ClickDetector->Reshape(width, height);
	m_AnimalMenu->Reshape(width, height);
}

void My_Timer(int val){
	glutPostRedisplay();
	glutTimerFunc(timer_speed, My_Timer, val);
}
void CreateObject(int _ix, int _iy) {
	GLint iWidth, internalFormat;
	GLint iHeight;
	glBindTexture(GL_TEXTURE_2D, m_ClickDetector->GetTextureID());
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &internalFormat);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iWidth); // get width of GL texture

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iHeight);

	//printf("Image Size : (%d, %d)\n", iWidth, iHeight);
	unsigned short *pixels = (unsigned short*)malloc(iWidth * iHeight * 4 * 2);
	glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_UNSIGNED_SHORT, pixels);
	int iIndex = (_ix + (iHeight - _iy) * iWidth) * 4;
	PrintVector3("Point : ", vec3(pixels[iIndex], pixels[iIndex + 1], pixels[iIndex + 2]));
	float fTerrainSize = m_ClickDetector->GetScalar();
	vec4 v4WorldPosition = ((vec4(pixels[iIndex], pixels[iIndex + 1], pixels[iIndex + 2], pixels[iIndex + 3]) / 65535.f) * fTerrainSize) - fTerrainSize * 0.5f;
	
	printf("Click World Position : (%f, %f, %f, %f)\n", v4WorldPosition.x, v4WorldPosition.y, v4WorldPosition.z, v4WorldPosition.w);

	if (pixels[iIndex + 3] != 0) {
		GameObject *NewGameObject = m_AnimalManager->CreateAnimal(m_SelectAnimalType);
		NewGameObject->SetWorldPosition(v4WorldPosition.xyz);
		NewGameObject->RefreshPositionAndYaw();
		m_ListGameObject.push_back(NewGameObject);
		m_Fluid->AddGameObject(NewGameObject);
		m_MovementSimulator->AddToAIController(NewGameObject, v4WorldPosition.xz, 200);
	}
	else {
		printf("Click OutSize the terrain!!\n");
	}
	
	//float TextureData[iWidth * iHeight * 4];
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);
	//printf("Size : %d", sizeof(TextureData) / sizeof(float));
}
void My_Mouse(int button, int state, int x, int y){
	myMouse.MouseFunc(button, state, x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		//printf("Press!!!");
		printf("Mouse Position (%d, %d)\n", x, y);
		bool bClick = m_AnimalMenu->MouseClick(x, y);
		//m_Fluid->CheckHit(m_FirstPersonCamera->GetEyePosition(), m_FirstPersonCamera->GetForwardDirection(), 1000);
		if (bClick) {
			m_SelectAnimalType = m_AnimalMenu->GetSelectedAnimalType();
		}
		else {
			if (m_bEnableCreateObject) {
				if (m_bMoveMouse) {
					CreateObject(m_ImageSize.x*0.5f, m_ImageSize.y*0.5f);
				}
				else {
					CreateObject(x, y);
				}
				//CreateObject(m_ImageSize.x*0.5f, m_ImageSize.y*0.5f);
				//CreateObject(x, y);
			}
		}
	}
};

void My_Motion(int x, int y){
	if (myMouse.MotionFunc(x,y)){
		myCamera.turnHeadDrag(myMouse.getDiff() / Config.win_size * vec2(-1));
	}
}
void My_PassiveMotion(int x, int y) {
	myCamera.turnHeadPassive(vec2(x, y)/Config.win_size * vec2(-2) + vec2(1));
	m_AnimalMenu->SetMousePosition(vec2(x, y));
	if (m_bMoveMouse) {
		m_FirstPersonCamera->MouseMotion(x, y);
		m_ViewMatrix = m_FirstPersonCamera->GetViewMatrix();
	}
}

void My_KeyboardUp(unsigned char key, int x, int y) {
	m_MovementSimulator->UpdateKeyBoardUp(key, x, y);
	m_FirstPersonCamera->KeyBoardUp(key);
}

void My_Keyboard(unsigned char key, int x, int y){
	m_MovementSimulator->UpdateKeyBoardDown(key,x,y);
	myKeyboard.receiveClick( key,  x,  y);
	m_FirstPersonCamera->KeyBoard(key);
	m_AnimalMenu->UpdateKeyBoardDown(key);
	m_SelectAnimalType = m_AnimalMenu->GetSelectedAnimalType();
	switch (key)
	{
	case 'w':
	case 'W':
	case 'a':
	case 'A':
	case 's':
	case 'S':
	case 'd':
	case 'D':
	case 'F':
	case 'f':
	case ' ': 
	case 'P':
		break;
	case 'p':
	{
		float terrainHeight = terrain.getHeight(myCamera) + 5.0;
		myCamera.move(myKeyboard, terrainHeight);
		break;
	}
	case 'r':
	case 'R':
		terrain.reInitialize(rand());
		break;
	case 'k':
		m_bMoveMouse = !m_bMoveMouse;
		if (m_bMoveMouse) {
			glutSetCursor(GLUT_CURSOR_NONE);
			m_FirstPersonCamera->SetOldMousePosition(x, y);
		}
		else {
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}

		break;
	case 'o':
		terrain.changeSeasonDirection(-1 * terrain.getSeasonDir());
		break;
	case 'i':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'M':
	case 'm':
		trees.OpenSnow();
		houses.OpenSnow();
		snowfall.OpenSnow();
		bridge.OpenSnow();
		pot.OpenSnow();
		break;
	case 'N':
	case 'n':
		trees.CloseSnow();
		houses.CloseSnow();
		snowfall.CloseSnow();
		bridge.CloseSnow();
		pot.CloseSnow();
		break;
	case '/':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'y':
		break;
	case 'z':
		m_bEnableCreateObject = !m_bEnableCreateObject;
		break;
	default:
		break;

	}
};

void My_SpecialKeys(int key, int x, int y){};

void My_Menu(int id){
	switch (id){
	case MENU_TIMER_START:
		if (!timer_enabled){
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	default:
		break;
	}
}

void environtSetting(int argc, char**argv){
	#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
	#endif
		// Initialize GLUT and GLEW, then create a window.
		////////////////////
		glutInit(&argc, argv);
	#ifdef _MSC_VER
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	#else
		glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	#endif
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(600, 600);
		glutCreateWindow("AS3_Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	#ifdef _MSC_VER
		glewInit();
	#endif
		glPrintContextInfo();
}
int main(int argc, char *argv[])
{

	environtSetting(argc,argv);
	My_Init();

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutKeyboardUpFunc(My_KeyboardUp);
	glutSpecialFunc(My_SpecialKeys);
	glutMotionFunc(My_Motion);
	glutPassiveMotionFunc(My_PassiveMotion);
	glutTimerFunc(timer_speed, My_Timer, 0);
	// Enter main event loop.
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutMainLoop();

	return 0;
}
