#include "geometria.h"
#include "RFPLevelScene.h"
#include "../Physics/Colliders/BoxCollider.h"
#include "../Physics/Rigidbody/Rigidbody.h"
#include "Scripts/Player/PlayerController.h"
#include "Scripts/GeometryDash/GDLevelLoader.h"
#include "../Audio/AudioSource.h"

Model* RFPPlayer = NULL,
* RFPBaseGround = NULL;
DrawCall* UIDrawCall = NULL;

void RFPLevelScene::Init()
{
	SceneManager::MainScene().MainDrawCall()->type = DrawCall::Type::Dynamic;
	SceneManager::MainScene().MainDrawCall()->sort = DrawCall::Sorting::AtStartup;
	SceneManager::MainScene().MainDrawCall()->mode = DrawCall::Mode::To2D;
	//Model Initialization

	Graphics::MainCamera()->transform.position = Vector3(0, 10, 6.5);

	RFPPlayer = new Model(Model::Square(), Vector3(0, 0.5, 0), Vector3(0), Vector3(1));
	RFPPlayer->color = Color::yellow();
	RFPPlayer->AddScript<BoxCollider>();
	RFPPlayer->AddScript<Rigidbody>();
	RFPPlayer->GetScript<Rigidbody>()->freezePositionZ = true;
	RFPPlayer->GetScript<Rigidbody>()->freezePositionX = true;
	RFPPlayer->AddScript<PlayerController>();
	RFPPlayer->GetScript<PlayerController>()->camera = Graphics::MainCamera();
	RFPPlayer->AddScript<AudioSource>();
	std::cout << "Player Added!" << std::endl;
	RendererCore::AddModel(*RFPPlayer);

	/*UIDrawCall = SceneManager::MainScene().CreateDrawCall();
	UIDrawCall->sort = DrawCall::Sorting::AtStartup;
	UIDrawCall->type = DrawCall::Type::UI;
	UIDrawCall->objectClassName = "UI Draw Call";
	UIDrawCall->Close();

	ImGUIElement* newGUIML = GUIML::NewGUIML("", "");
	RendererCore::AddImGUIElement(*newGUIML, UIDrawCall->Target());*/

	Empty* GDLoader = new Empty();
	GDLoader->AddScript<GDLevelLoader>();

	/*Model* s1 = new Model(Model::Square(), Vector3(25, -8, 0), Vector3(0), Vector3(8, 3, 10));
	s1->AddScript<BoxCollider>();
	RendererCore::AddModel(*s1);*/

	RFPBaseGround = new Model(Model::Square(), Vector3(0, -2, 0), Vector3(0), Vector3(10000, 4, 1000));
	RFPBaseGround->AddScript<BoxCollider>();
	RendererCore::AddModel(*RFPBaseGround);
}
