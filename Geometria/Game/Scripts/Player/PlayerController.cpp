#include "PlayerController.h"
#include "../Editor/Editor.h"

VisualAccess(PlayerController);

void PlayerController::OnStartup()
{
	ClassType = Class::Script;
}

void PlayerController::OnStart()
{
	RequireStaticScript(PhysicsManager);

	if (GetScript<Rigidbody>() == nullptr)
		rb = AddScript<Rigidbody>();
	else
		rb = GetScript<Rigidbody>();

	PhysicsManager::SetGravity(Vector3(0, -35, 0));
}

void PlayerController::OnUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_SPACE))
	{
		if(reverse)
			rb->SetVelocity(Vector3(0, -15, 0));
		else
			rb->SetVelocity(Vector3(0, 15, 0));
	}

	GetTransform().position += Vector3(7 * Graphics::DeltaTime(), 0, 0);

	if (Input::GetKeyDown(GLFW_KEY_Q))
	{
		reverse = !reverse;

		if (reverse)
			PhysicsManager::SetGravity(Vector3(0, 35, 0));
		else
			PhysicsManager::SetGravity(Vector3(0, -35, 0));
	}

	if (camera != nullptr)
	{
		camera->GetTransform().position = Vector3(GetTransform().position.x, -5, GetTransform().position.z + 20);
	}
}

void PlayerController::OnInspector()
{
	VisualAccess_Title(PlayerController);
}

void PlayerController::OnSave()
{
	SaveNewScript(PlayerController);
}
