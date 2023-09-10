#include "Components.h"
#include "CookieEngine.h"
#include "Systems.h"

using namespace Cookie;

// Sprites
// --------------------------------------------------------------------------
static SpriteHandle sugarCube;
static SpriteHandle butterStick;

static SpriteHandle cookieSprite;
static SpriteHandle bigCookieSprite;

// Entities Templates
// --------------------------------------------------------------------------
void CreatePlayer(EntityAdmin *const EntitiesAdmin, Float3 pos) {
	EntityID e = EntitiesAdmin->CreateEntity();

	TransformComponent transform = {};
	RenderComponent render = {};
	PlayerCharacterComponent move = {};
	AttackComponent attack = {};

	attack.m_MinArea = 1.5f;
	attack.m_LastAttackID = 0;
	attack.m_MaxArea = 2.0f;
	attack.m_RectX = 3.0f;
	attack.m_RectY = 0.35f;
	attack.m_CooldownTotal = 1.0f;
	attack.m_CooldownElapsed = 0.0f;

	transform.m_Position = pos;
	render.m_SpriteHandle = bigCookieSprite;
	move.m_Speed = 3.0f;

	EntitiesAdmin->AddComponent(e, transform);
	EntitiesAdmin->AddComponent(e, render);
	EntitiesAdmin->AddComponent(e, move);
	EntitiesAdmin->AddComponent(e, attack);
};

void CreateEnemy(EntityAdmin *const EntitiesAdmin, Float3 pos, SpriteHandle sprite) {
	EntityID e = EntitiesAdmin->CreateEntity();

	TransformComponent transform = {};
	RenderComponent render = {};
	EnemyComponent enemy = {};

	transform.m_Position = pos;
	render.m_SpriteHandle = sprite;
	enemy.m_Acceleration = Random::Float(1.0f, 3.0f);
	enemy.m_TopSpeed = Random::Float(1.5f, 2.5f);

	EntitiesAdmin->AddComponent(e, transform);
	EntitiesAdmin->AddComponent(e, render);
	EntitiesAdmin->AddComponent(e, enemy);
};

void CreateCamera(EntityAdmin *const EntitiesAdmin) {
	auto mainCam = EntitiesAdmin->CreateEntity();
	CameraComponent camComp;
	camComp.m_Position = Float3(0.0f, 0.0f, 7.5f);
	camComp.m_Rotation = 0.0f;
	EntitiesAdmin->AddComponent(mainCam, camComp);
	EntitiesAdmin->GetSinglComponent<CameraComponentSingl>()->m_MainCam = mainCam;
}

// Worlds Templates
// --------------------------------------------------------------------------
void MixedWorld(EntityAdmin *const EntitiesAdmin) {
	i32 rows = 1000;
	i32 columns = 100;
	for (size_t x = 0; x < columns; x++) {

		SpriteHandle sprite;
		if (x < 33) {
			sprite = cookieSprite;
		} else if (x < 66) {
			sprite = sugarCube;
		} else {
			sprite = butterStick;
		}

		for (size_t y = 0; y < rows; y++) {
			f32 zPos = Random::Float(0.001f, 0.01f);
			f32 xPos = -20.0f + (40.0f / (f32)columns) * x;
			f32 yPos = -20.0f + (40.0f / (f32)rows) * y;

			if (!(xPos < 1.0f && xPos > -1.0f && yPos < 1.0f && yPos > -1.0f)) {
				CreateEnemy(EntitiesAdmin, Float3(xPos, yPos, zPos), sprite);
			}
		}
	}
}

void CookiesWorld(EntityAdmin *const EntitiesAdmin) {
	i32 rows = 100;
	i32 columns = 100;
	for (size_t x = 0; x < columns; x++) {
		for (size_t y = 0; y < rows; y++) {
			CreateEnemy(EntitiesAdmin, Float3(-0.5f + (1.0f / (f32)columns) * x, -0.5f + (1.0f / (f32)rows) * y, -0.001f), cookieSprite);
		}
	}
}

// Game Initialization
// --------------------------------------------------------------------------
void LoadResources() {
	CKE_PROFILE_EVENT();
	// Resources
	ImageHandle cookieImg = ResourcesSystem::LoadImage("cookie.png");
	ImageHandle butterImg = ResourcesSystem::LoadImage("butter_stick.png");
	ImageHandle sugarImg = ResourcesSystem::LoadImage("sugar_cube.png");

	cookieSprite = ResourcesSystem::GenerateSprite(cookieImg, 32.0f * 8.0f);
	bigCookieSprite = ResourcesSystem::GenerateSprite(cookieImg, 32.0f * 2.5f);
	butterStick = ResourcesSystem::GenerateSprite(butterImg, 32.0f * 13.0f);
	sugarCube = ResourcesSystem::GenerateSprite(sugarImg, 32.0f * 8.0f);
}

void RegisterComponents(EntityAdmin *const EntitiesAdmin) {

	// Register Components
	EntitiesAdmin->RegisterComponent<PlayerCharacterComponent>();
	EntitiesAdmin->RegisterComponent<EnemyComponent>();
	EntitiesAdmin->RegisterComponent<AttackComponent>();

	EntitiesAdmin->RegisterSinglComponent<ScoreSinglComponent>();
	EntitiesAdmin->RegisterSinglComponent<SinglMainPlayerComponent>();

	// Register Systems in order of execution
	EntitiesAdmin->RegisterSystem<PlayerMovementSystem>();
	EntitiesAdmin->RegisterSystem<CameraSystem>();
	EntitiesAdmin->RegisterSystem<AttackSystem>();
	EntitiesAdmin->RegisterSystem<EnemySystem>();
}

void CreateWorld(EntityAdmin *const EntitiesAdmin) {
	CKE_PROFILE_EVENT();
	using namespace Cookie;

	// Create World Entities
	CreateCamera(EntitiesAdmin);
	CreatePlayer(EntitiesAdmin, Float3(0.0f, 0.0f, 1.0f));
	MixedWorld(EntitiesAdmin);
}

// Entry Point
// --------------------------------------------------------------------------

int main() {
	using namespace Cookie;
	GameInitData g = {};

	WindowDescription wd = {};
	wd.m_Title = "Cookie Survival";
	wd.m_Width = 1280;
	wd.m_Height = 720;
	wd.m_IsFullScreen = false;
	wd.m_RefreshRate = 144;

	g.m_WindowDesc = wd;
	g.m_LoadResourcesFunc = LoadResources;
	g.m_CreateWorldFunc = CreateWorld;
	g.m_RegisterECSFunc = RegisterComponents;

	Application::Run(&g);
	return 0;
}