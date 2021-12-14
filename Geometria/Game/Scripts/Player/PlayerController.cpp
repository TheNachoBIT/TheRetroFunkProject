#include "PlayerController.h"
#include "../Editor/Editor.h"
#include "Audio/AudioSource.h"

VisualAccess(PlayerController);

void PlayerController::OnStartup()
{
	ClassType = Class::Script;
}

void PlayerController::OnStart()
{
	RequireStaticScript(PhysicsManager);

	rb = GetScript<Rigidbody>();

	camera = Graphics::MainCamera();

	rb->freezePositionX = true;
	rb->freezePositionZ = true;

	PhysicsManager::SetGravity(Vector3(0, -25, 0));
	GetTransform().position = Vector3(-7, 0.5, 0);

	offset = Vector3(2, 0, 11);
}

void PlayerController::CameraUpdate()
{
	if (camera != nullptr)
	{
		if (GetTransform().position.y > (cameraY + 4))
			cameraY += 1;
		
		if (GetTransform().position.y < (cameraY - 2))
			cameraY -= 1;

		if (cameraY < 2.5)
			cameraY = 2.5;

		Vector3 result = Vector3(GetTransform().position.x + offset.x, cameraY + offset.y, GetTransform().position.z + offset.z);
		Vector3 follow = Vector3::Lerp(camera->GetTransform().position, result, 0.05f);
		//std::cout << follow.x << " || " << follow.y << " || " << follow.z << std::endl;
		camera->GetTransform().position = Vector3(result.x, follow.y, result.z);
	}
}

void PlayerController::OnUpdate()
{
	if(canStart)
	{
		if (PhysicsManager::Raycast(Vector3(GetTransform().position.x, GetTransform().position.y - 0.55f, GetTransform().position.z), Vector3::down(), 0.01))
		{
			if (Input::GetKey(GLFW_KEY_SPACE))
			{
				if (reverse)
					rb->SetVelocity(Vector3(0, -11, 0));
				else
					rb->SetVelocity(Vector3(0, 11, 0));
			}
		}
		GetTransform().position += Vector3(5.2 * Graphics::DeltaTime(), 0, 0);

		if (Input::GetKeyDown(GLFW_KEY_Q))
		{
			reverse = !reverse;

			if (reverse)
				PhysicsManager::SetGravity(Vector3(0, 30, 0));
			else
				PhysicsManager::SetGravity(Vector3(0, -30, 0));
		}

		if (GetTransform().position.y < -2)
		{
			rb->GetRigidbodyTransform().position = Vector3(GetTransform().position.x, 0.5, GetTransform().position.z);
		}
	}
	else if (Input::GetKeyDown(GLFW_KEY_ENTER))
	{
		GetScript<AudioSource>()->Play();
		canStart = true;
	}

	CameraUpdate();
}

void PlayerController::OnInspector()
{
	VisualAccess_Title(PlayerController);
}

void PlayerController::OnSave()
{
	SaveInclude(PlayerController, "Scripts/Player/PlayerController.h");
	SaveNewScript(PlayerController);

	SaveValue(speed);
	SaveInstruction(
		CurrentObject::speed = SaveReadInt(CurrentObject::speed);
	);

	SaveEnd();
}

void PlayerController::OnCollisionEnter()
{
	isGrounded = true;
}

void PlayerController::OnCollisionExit()
{
	isGrounded = false;
}
