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
	std::vector<Texture*> GDTextures;
	void ReadLevel(std::string rawData);
};