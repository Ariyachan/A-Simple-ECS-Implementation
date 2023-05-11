#include "Components/Camera.hpp"
#include "Components/Gravity.hpp"
#include "Components/Player.hpp"
#include "Components/Renderable.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Thrust.hpp"
#include "Components/Transform.hpp"
#include "Core/Coordinator.hpp"
#include "Systems/CameraControlSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControlSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "WindowManager.hpp"
#include <chrono>
#include <random>


Coordinator gCoordinator;

static bool quit = false;


void QuitHandler(Event& event)
{
	quit = true;
}

int main()
{
	gCoordinator.Init();


	WindowManager windowManager;
	windowManager.Init("Nexus", 1920, 1080, 0, 0);


	gCoordinator.AddEventListener(FUNCTION_LISTENER(Events::Window::QUIT, QuitHandler));


	gCoordinator.RegisterComponent<Camera>();
	gCoordinator.RegisterComponent<Gravity>();
	gCoordinator.RegisterComponent<Player>();
	gCoordinator.RegisterComponent<Renderable>();
	gCoordinator.RegisterComponent<RigidBody>();
	gCoordinator.RegisterComponent<Thrust>();
	gCoordinator.RegisterComponent<Transform>();


	auto physicsSystem = gCoordinator.RegisterSystem<PhysicsSystem>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType<Gravity>());
		signature.set(gCoordinator.GetComponentType<RigidBody>());
		signature.set(gCoordinator.GetComponentType<Transform>());
		gCoordinator.SetSystemSignature<PhysicsSystem>(signature);
	}

	physicsSystem->Init();


	auto cameraControlSystem = gCoordinator.RegisterSystem<CameraControlSystem>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType<Camera>());
		signature.set(gCoordinator.GetComponentType<Transform>());
		gCoordinator.SetSystemSignature<CameraControlSystem>(signature);
	}

	cameraControlSystem->Init();


	auto playerControlSystem = gCoordinator.RegisterSystem<PlayerControlSystem>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType<Player>());
		signature.set(gCoordinator.GetComponentType<Transform>());
		gCoordinator.SetSystemSignature<PlayerControlSystem>(signature);
	}

	playerControlSystem->Init();


	auto renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
	{
		Signature signature;
		signature.set(gCoordinator.GetComponentType<Renderable>());
		signature.set(gCoordinator.GetComponentType<Transform>());
		gCoordinator.SetSystemSignature<RenderSystem>(signature);
	}

	renderSystem->Init();

	std::vector<Entity> entities(MAX_ENTITIES - 1);

	std::default_random_engine generator;
	std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
	std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
	std::uniform_real_distribution<float> randScale(3.0f, 5.0f);
	std::uniform_real_distribution<float> randColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);

	float scale = randScale(generator);

	for (auto& entity : entities)
	{
		entity = gCoordinator.CreateEntity();
		gCoordinator.AddComponent(entity, Player{});

		gCoordinator.AddComponent<Gravity>(
			entity,
			{Vec3(0.0f, randGravity(generator), 0.0f)});

		gCoordinator.AddComponent(
			entity,
			RigidBody{
				.velocity = Vec3(0.0f, 0.0f, 0.0f),
				.acceleration = Vec3(0.0f, 0.0f, 0.0f)
			});

		gCoordinator.AddComponent(
			entity,
			Transform{
				.position = Vec3(randPosition(generator), randPosition(generator), randPosition(generator)),
				.rotation = Vec3(randRotation(generator), randRotation(generator), randRotation(generator)),
				.scale = Vec3(scale, scale, scale)
			});

		gCoordinator.AddComponent(
			entity,
			Renderable{
				.color = Vec3(randColor(generator), randColor(generator), randColor(generator))
			});
	}

	float dt = 0.0f;

	while (!quit)
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		windowManager.ProcessEvents();

		playerControlSystem->Update(dt);

		cameraControlSystem->Update(dt);

		physicsSystem->Update(dt);

		renderSystem->Update(dt);

		windowManager.Update();

		auto stopTime = std::chrono::high_resolution_clock::now();

		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
	}


	windowManager.Shutdown();

	return 0;
}
