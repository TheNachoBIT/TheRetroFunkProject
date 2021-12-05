#include "SampleScene.tits.h"
#include "geometria.h"
#include "Scripts/Player/PlayerController.h"

Model* Model6;

void SampleScene::Init()
{
SceneSaveAndLoad::StartSceneLoad("Game/SampleScene.tits");
TextureManager::UploadToGPU();

SceneManager::MainScene().MainDrawCall()->sort = (DrawCall::Sorting)SaveReadInt(SceneManager::MainScene().MainDrawCall()->sort); SceneManager::MainScene().MainDrawCall()->type = (DrawCall::Type)SaveReadInt(SceneManager::MainScene().MainDrawCall()->type);

SceneSaveAndLoad::StartLoadArray("Model6");
Model6 = new Model(Model::Square(), Vector4(SaveReadFloat(Model6->transform.position.x), SaveReadFloat(Model6->transform.position.y), SaveReadFloat(Model6->transform.position.z), SaveReadFloat(Model6->transform.position.w)), Vector4(SaveReadFloat(Model6->transform.rotation.x), SaveReadFloat(Model6->transform.rotation.y), SaveReadFloat(Model6->transform.rotation.z), SaveReadFloat(Model6->transform.rotation.w)), Vector4(SaveReadFloat(Model6->transform.scale.x), SaveReadFloat(Model6->transform.scale.y), SaveReadFloat(Model6->transform.scale.z), SaveReadFloat(Model6->transform.scale.w)));
Model6->objectClassName = "Model41";
Model6->color = Color(SaveReadFloat(Model6->color.r), SaveReadFloat(Model6->color.g), SaveReadFloat(Model6->color.b), SaveReadFloat(Model6->color.a));
RendererCore::AddModel(*Model6);
Model6->AddScript<PlayerController>();
SceneSaveAndLoad::EndLoadArray();
}