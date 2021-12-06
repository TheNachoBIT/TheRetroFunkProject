#pragma once

#include "Behaviour.h"
#include "geometria.h"
#include "../../../Physics/Rigidbody/Rigidbody.h"

struct PlayerController : public ScriptBehaviour
{
	AllowCloning(PlayerController);

	void OnStartup();

	void OnStart();
	void OnUpdate();

	void OnCollisionEnter();

	void OnCollisionExit();

	void OnInspector();

	void OnSave();

	bool isGrounded = false;
	Camera* camera = nullptr;
	bool reverse = false;
	Rigidbody* rb = nullptr;
};
