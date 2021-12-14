#pragma once

#include "Behaviour.h"
#include "geometria.h"
#include "../../../Physics/Rigidbody/Rigidbody.h"

struct PlayerController : public ScriptBehaviour
{
	AllowCloning(PlayerController);

	void OnStartup();

	void OnStart();
	void CameraUpdate();
	void OnUpdate();

	void OnCollisionEnter();

	void OnCollisionExit();

	void OnInspector();

	void OnSave();

	float interpVelocity = 0;
	Vector3 offset, targetPos;
	float cameraY = 1.5f;
	bool isGrounded = false;
	Camera* camera = nullptr;
	bool reverse = false;
	bool audioPlayed = false;
	bool canStart = false;
	int speed = 6;
	Rigidbody* rb = nullptr;
};
