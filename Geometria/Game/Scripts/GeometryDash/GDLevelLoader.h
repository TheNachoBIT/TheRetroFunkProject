#pragma once

#ifndef BEHAVIOUR_H
#include "Behaviour.h"
#endif

#ifndef GEOMETRIA_H
#include "geometria.h"
#endif

struct GDLevelLoader : public ScriptBehaviour
{
	void OnStart();

	std::vector<std::pair<int, Vector3>> customSize;
	std::vector<int> noCollisions;
	std::vector<std::pair<int, Vector3>> customCollisionSize;

	std::vector<Texture*> GDTextures;
	void ReadLevel(std::string rawData);
	void LoadCustomProperties();

	void AddCustomSize(int id, Vector3 size);
	void AddNoCollisions(int id);
	void AddCustomCollisionSize(int id, Vector3 size);

	std::string username, password;

	void LoadVerifier();
	void SetupScene();
};