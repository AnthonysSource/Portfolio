#include "Components.h"
#include "CookieEngine.h"
#include "Systems.h"

using namespace Cookie;

// Sprites
// --------------------------------------------------------------------------
SpriteHandle cookieSprite;

// Entities Templates
// --------------------------------------------------------------------------
void CreatePhysicsBall(EntityAdmin *const EntitiesAdmin, Float3 pos) {
	EntityID e = EntitiesAdmin->CreateEntity();

	TransformComponent transform = {};
	RenderComponent render = {};
	PhysicsComponent physics = {};

	b2World *physicsWorld = EntitiesAdmin->GetSinglComponent<PhysicsWorldSinglComponent>()->m_World;

	transform.m_Position = pos;
	render.m_SpriteHandle = cookieSprite;
	b2BodyDef circleBodyDef;
	circleBodyDef.type = b2_dynamicBody;
	circleBodyDef.position.Set(pos.x, pos.y);
	physics.m_Body = physicsWorld->CreateBody(&circleBodyDef);
	b2PolygonShape circle;
	circle.SetAsBox(0.1f, 0.1f);
	b2FixtureDef fixture;
	fixture.shape = &circle;
	fixture.density = Random::Float(0.8f, 2.0f);
	fixture.friction = 0.3f;
	fixture.restitution = 1.0f;

	physics.m_Body->CreateFixture(&fixture);

	render.m_SpriteHandle = cookieSprite;

	EntitiesAdmin->AddComponent(e, transform);
	EntitiesAdmin->AddComponent(e, render);
	EntitiesAdmin->AddComponent(e, physics);
};

void CreatePhysicsGround(EntityAdmin *const EntitiesAdmin, Float3 pos) {
	EntityID e = EntitiesAdmin->CreateEntity();

	TransformComponent transform = {};
	RenderComponent render = {};
	PhysicsComponent physics = {};

	b2World *physicsWorld = EntitiesAdmin->GetSinglComponent<PhysicsWorldSinglComponent>()->m_World;

	transform.m_Position = pos;
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(pos.x, pos.y);
	physics.m_Body = physicsWorld->CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 1.0f);
	physics.m_Body->CreateFixture(&groundBox, 0.0f);

	EntitiesAdmin->AddComponent(e, transform);
	EntitiesAdmin->AddComponent(e, physics);
};

void CreateCamera(EntityAdmin *const EntitiesAdmin) {
	auto mainCam = EntitiesAdmin->CreateEntity();
	CameraComponent camComp;
	camComp.m_Position = Float3(0.0f, 0.0f, 15.0f);
	camComp.m_Rotation = 0.0f;
	EntitiesAdmin->AddComponent(mainCam, camComp);
	EntitiesAdmin->GetSinglComponent<CameraComponentSingl>()->m_MainCam = mainCam;
}

// Game Initialization
// --------------------------------------------------------------------------
void LoadResources() {
	CKE_PROFILE_EVENT();
	// Resources
	ImageHandle cookieImg = ResourcesSystem::LoadImage("cookie.png");
	ImageHandle butterImg = ResourcesSystem::LoadImage("butter_stick.png");
	ImageHandle sugarImg = ResourcesSystem::LoadImage("sugar_cube.png");

	cookieSprite = ResourcesSystem::GenerateSprite(cookieImg, 32.0f);
}

void RegisterECS(EntityAdmin *const EntitiesAdmin) {
	using namespace Cookie;

	// Register Components
	EntitiesAdmin->RegisterComponent<PlayerCharacterComponent>();
	EntitiesAdmin->RegisterComponent<EnemyComponent>();

	EntitiesAdmin->RegisterSinglComponent<SinglMainPlayerComponent>();

	// Register Systems in order of execution
	EntitiesAdmin->RegisterSystem<PlayerMovementSystem>();
	EntitiesAdmin->RegisterSystem<EnemySystem>();
	EntitiesAdmin->RegisterSystem<CameraSystem>();
}

void CreateWorld(EntityAdmin *const EntitiesAdmin) {
	CKE_PROFILE_EVENT();
	using namespace Cookie;

	CreateCamera(EntitiesAdmin);
	CreatePhysicsGround(EntitiesAdmin, Float3(0.0f, -8.5f, 0.0f));

	i32 rows = 500;
	for (size_t x = 0; x < rows; x++) {
		CreatePhysicsBall(EntitiesAdmin, Float3(-20.0f + ((f32)x / rows) * 40.0f, 10.0f + Random::Float(-5.0f, 5.0f), 0.0f));
	}
}

// Entry Point
// --------------------------------------------------------------------------
int main() {
	using namespace Cookie;
	GameInitData g = {};

	WindowDescription wd = {};
	wd.m_Title = "Cookie Physics";
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