#include "SampleScene.tits.h"
#include "geometria.h"

#ifndef BoxCollider_SCENE
#define BoxCollider_SCENE
#include "Physics/Colliders/BoxCollider.h"
#endif
#ifndef PlayerController_SCENE
#define PlayerController_SCENE
#include "Scripts/Player/PlayerController.h"
#endif
#ifndef Rigidbody_SCENE
#define Rigidbody_SCENE
#include "Physics/Rigidbody/Rigidbody.h"
#endif

Model* Model6 = nullptr;
BoxCollider* BoxCollider2 = nullptr;
Model* Model7 = nullptr;
PlayerController* PlayerController8 = nullptr;
BoxCollider* BoxCollider9 = nullptr;
Rigidbody* Rigidbody10 = nullptr;

void SampleScene::Init()
{
SceneSaveAndLoad::StartSceneLoad("Game/SampleScene.tits");
TextureManager::UploadToGPU();

SceneManager::MainScene().MainDrawCall()->sort = (DrawCall::Sorting)SaveReadInt(SceneManager::MainScene().MainDrawCall()->sort); SceneManager::MainScene().MainDrawCall()->type = (DrawCall::Type)SaveReadInt(SceneManager::MainScene().MainDrawCall()->type);

SceneSaveAndLoad::StartLoadArray("Model6");
Model6 = new Model(Model::Square(), Vector4(SaveReadFloat(Model6->transform.position.x), SaveReadFloat(Model6->transform.position.y), SaveReadFloat(Model6->transform.position.z), SaveReadFloat(Model6->transform.position.w)), Vector4(SaveReadFloat(Model6->transform.rotation.x), SaveReadFloat(Model6->transform.rotation.y), SaveReadFloat(Model6->transform.rotation.z), SaveReadFloat(Model6->transform.rotation.w)), Vector4(SaveReadFloat(Model6->transform.scale.x), SaveReadFloat(Model6->transform.scale.y), SaveReadFloat(Model6->transform.scale.z), SaveReadFloat(Model6->transform.scale.w)));
Model6->objectClassName = "Ground";
Model6->color = Color(SaveReadFloat(Model6->color.r), SaveReadFloat(Model6->color.g), SaveReadFloat(Model6->color.b), SaveReadFloat(Model6->color.a));
RendererCore::AddModel(*Model6);
BoxCollider2 = Model6->AddScript<BoxCollider>();
SceneSaveAndLoad::EndLoadArray();

SceneSaveAndLoad::StartLoadArray("Model7");
Model7 = new Model(Model::Square(), Vector4(SaveReadFloat(Model7->transform.position.x), SaveReadFloat(Model7->transform.position.y), SaveReadFloat(Model7->transform.position.z), SaveReadFloat(Model7->transform.position.w)), Vector4(SaveReadFloat(Model7->transform.rotation.x), SaveReadFloat(Model7->transform.rotation.y), SaveReadFloat(Model7->transform.rotation.z), SaveReadFloat(Model7->transform.rotation.w)), Vector4(SaveReadFloat(Model7->transform.scale.x), SaveReadFloat(Model7->transform.scale.y), SaveReadFloat(Model7->transform.scale.z), SaveReadFloat(Model7->transform.scale.w)));
Model7->objectClassName = "Player";
Model7->color = Color(SaveReadFloat(Model7->color.r), SaveReadFloat(Model7->color.g), SaveReadFloat(Model7->color.b), SaveReadFloat(Model7->color.a));
RendererCore::AddModel(*Model7);
BoxCollider9 = Model7->AddScript<BoxCollider>();
Rigidbody10 = Model7->AddScript<Rigidbody>();
PlayerController8 = Model7->AddScript<PlayerController>();
PlayerController8->speed = SaveReadInt(PlayerController8->speed);
SceneSaveAndLoad::EndLoadArray();
}