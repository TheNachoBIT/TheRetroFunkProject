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

	void OnInspector();

	void OnSave();

	Camera* camera = nullptr;
	bool reverse = false;
	Rigidbody* rb = nullptr;
};
