#include "Components.h"
#include "CookieEngine.h"
#include "Systems.h"

using namespace Cookie;

// Sprites
SpriteHandle cookieSprite;
SpriteHandle butterSprite;
SpriteHandle sugarSprite;

// Entities Templates
void CreateControllableCookie(EntityAdmin *const EntitiesAdmin, Float3 pos) {
	EntityID e = EntitiesAdmin->CreateEntity();

	TransformComponent transform = TransformComponent{};
	PlayerCharacterComponent move = PlayerCharacterComponent{};
	RotatingComponent rotating = RotatingComponent{};
	RenderComponent render = RenderComponent{};

	transform.m_Position = pos;
	rotating.m_Speed = 3.14f;
	move.m_Speed = 4.0f;
	render.m_SpriteHandle = cookieSprite;

	EntitiesAdmin->AddComponent(e, transform);
	EntitiesAdmin->AddComponent(e, render);
	EntitiesAdmin->AddComponent(e, rotating);
	EntitiesAdmin->AddComponent(e, move);
};

void CreateFloatingButterStick(EntityAdmin *const EntitiesAdmin, Float3 pos) {
	EntityID e = EntitiesAdmin->CreateEntity();

	TransformComponent transform = TransformComponent{};
	RotatingComponent rotating = RotatingComponent{};
	FloatComponent floating = FloatComponent{};
	RenderComponent render = RenderComponent{};

	transform.m_Position = pos;
	rotating.m_Speed = 3.14f;
	floating.m_Speed = 1.0f;
	floating.m_Amplitude = 0.25f;
	render.m_SpriteHandle = butterSprite;

	EntitiesAdmin->AddComponent(e, transform);
	EntitiesAdmin->AddComponent(e, render);
	EntitiesAdmin->AddComponent(e, rotating);
	EntitiesAdmin->AddComponent(e, floating);
}

void CreateSugarCube(EntityAdmin *const EntitiesAdmin, Float3 pos) {
	EntityID e = EntitiesAdmin->CreateEntity();

	TransformComponent transform = TransformComponent{};
	RenderComponent render = RenderComponent{};

	transform.m_Position = pos;
	render.m_SpriteHandle = sugarSprite;

	EntitiesAdmin->AddComponent(e, transform);
	EntitiesAdmin->AddComponent(e, render);
}

void CreateCamera(EntityAdmin *const EntitiesAdmin) {
	auto mainCam = EntitiesAdmin->CreateEntity();
	CameraComponent camComp;
	camComp.m_Position = Float3(0.0f, 0.0f, 5.0f);
	camComp.m_Rotation = 0.0f;
	EntitiesAdmin->AddComponent(mainCam, camComp);
	EntitiesAdmin->GetSinglComponent<CameraComponentSingl>()->m_MainCam = mainCam;
}

void LoadResources() {
	CKE_PROFILE_EVENT();
	// Resources
	ImageHandle cookieImg = ResourcesSystem::LoadImage("cookie.png");
	ImageHandle butterImg = ResourcesSystem::LoadImage("butter_stick.png");
	ImageHandle sugarImg = ResourcesSystem::LoadImage("sugar_cube.png");

	cookieSprite = ResourcesSystem::GenerateSprite(cookieImg, 32.0f);
	butterSprite = ResourcesSystem::GenerateSprite(butterImg, 32.0f);
	sugarSprite = ResourcesSystem::GenerateSprite(sugarImg, 32.0f);
}

void RegisterECS(EntityAdmin *const EntitiesAdmin) {
	CKE_PROFILE_EVENT();

	using namespace Cookie;

	// Register Components
	EntitiesAdmin->RegisterComponent<RotatingComponent>();
	EntitiesAdmin->RegisterComponent<FloatComponent>();
	EntitiesAdmin->RegisterComponent<PlayerCharacterComponent>();

	// Register Systems in order of execution
	EntitiesAdmin->RegisterSystem<RotateSystem>();
	EntitiesAdmin->RegisterSystem<FloatSystem>();
	EntitiesAdmin->RegisterSystem<PlayerMovementSystem>();
}

// Game World Init
void CreateWorld(EntityAdmin *const EntitiesAdmin) {
	CKE_PROFILE_EVENT();

	using namespace Cookie;

	// Create World Entities
	CreateCamera(EntitiesAdmin);

	for (size_t x = 0; x < 100; x++) {
		for (size_t y = 0; y < 100; y++) {
			CreateControllableCookie(EntitiesAdmin, Float3(-4.0f + (8.0f / 100.0f) * x, -2.0f + (4.0f / 100.0f) * y, -0.001f));
		}
	}
}

int main() {
	using namespace Cookie;
	GameInitData g = {};

	WindowDescription wd = {};
	wd.m_Title = "Cookie Game";
	wd.m_Width = 1280;
	wd.m_Height = 720;
	wd.m_IsFullScreen = false;
	wd.m_RefreshRate = 144;

	g.m_WindowDesc = wd;
	g.m_LoadResourcesFunc = LoadResources;
	g.m_CreateWorldFunc = CreateWorld;
	g.m_RegisterECSFunc = RegisterECS;

	Application::Run(&g);
	return 0;
}