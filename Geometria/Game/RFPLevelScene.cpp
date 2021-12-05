#include "geometria.h"
#include "RFPLevelScene.h"
#include "../Physics/Colliders/BoxCollider.h"
#include "../Physics/Rigidbody/Rigidbody.h"
#include "Scripts/Player/PlayerController.h"
#include "Scripts/GeometryDash/GDLevelLoader.h"

Model* RFPPlayer = NULL,
* RFPBaseGround = NULL;

void RFPLevelScene::Init()
{
	SceneManager::MainScene().MainDrawCall()->type = DrawCall::Type::Dynamic;
	SceneManager::MainScene().MainDrawCall()->sort = DrawCall::Sorting::AtStartup;

	//Model Initialization

	Graphics::MainCamera()->transform.position = Vector3(0, -10, 30);

	RFPPlayer = new Model(Model::Square(), Vector3(0), Vector3(0), Vector3(1));
	RFPPlayer->color = Color::yellow();
	/*RFPPlayer->AddScript<BoxCollider>();
	RFPPlayer->AddScript<Rigidbody>();
	RFPPlayer->GetScript<Rigidbody>()->freezePositionZ = true;
	RFPPlayer->GetScript<Rigidbody>()->freezePositionX = true;
	RFPPlayer->AddScript<PlayerController>();
	RFPPlayer->GetScript<PlayerController>()->camera = Graphics::MainCamera();*/
	std::cout << "Player Added!" << std::endl;
	RendererCore::AddModel(*RFPPlayer);

	Empty* GDLoader = new Empty();
	GDLoader->AddScript<GDLevelLoader>();

	/*Model* s1 = new Model(Model::Square(), Vector3(25, -8, 0), Vector3(0), Vector3(8, 3, 10));
	s1->AddScript<BoxCollider>();
	RendererCore::AddModel(*s1);

	RFPBaseGround = new Model(Model::Square(), Vector3(0, -10, 0), Vector3(0), Vector3(100, 3, 100));
	RFPBaseGround->AddScript<BoxCollider>();
	RendererCore::AddModel(*RFPBaseGround);*/
}
