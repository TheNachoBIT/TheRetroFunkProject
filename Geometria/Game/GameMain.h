#pragma once

#include "RFPLevelScene.h"

struct GameMain
{
	static void Init()
	{
		// This is where everything starts in your game.
		// Feel free to change the scene, or add extra stuff before initializing.

		RFPLevelScene::Init();

		// In here you can enable the editor that comes included.
		// Feel free to comment it out when you don't need it.

		//Editor::Begin();
	}
};