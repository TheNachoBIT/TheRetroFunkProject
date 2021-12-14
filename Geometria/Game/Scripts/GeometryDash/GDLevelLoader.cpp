#include "GDLevelLoader.h"
#include "../../../Encryption/Encryption.h"
#include "../../../String/StringAPI.h"
#include "../../../Web/Web.h"
#include "../../../Physics/Colliders/BoxCollider.h"
//#include "Graphics/Cores/Texture/rectpack2D/finders_interface.h"

void GDLevelLoader::OnStart()
{
	/*WebRequest request("http://www.boomlings.com/database/downloadGJLevel22.php", WebRequest::HttpMethod::HTTP_POST);
	WebResponse response;
	WebForm form;

	form.AddField("gameVersion", "20");
	form.AddField("binaryVersion", "35");
	form.AddField("gdw", "0");
	form.AddField("levelID", "1650666");
	form.AddField("secret", "Wmfd2893gb7");

	request.SendWebRequest(&response, form);

	std::string urlResult = response.body;*/

	GDTextures = TextureManager::OpenTexturePack("Game/Textures/Blocks/blocks.gtxp");

	//TextureManager::OpenSpriteSheet("Game/Textures/Blocks/GJ_GameSheet.plist");

	// Stereo Madness: 1556066
	// How by Spu7nix: 63395980
	// Future Funk by JonathanGD: 44062068

	std::string urlResult = Web::Post("http://www.boomlings.com/database/downloadGJLevel22.php", "gameVersion=20&binaryVersion=35&gdw=0&levelID=1556066&secret=Wmfd2893gb7");

	std::string levelSubstring = StringAPI::GetSubstringBetween(urlResult, ":4:", ":5:");

	if (!StringAPI::StartsWith(levelSubstring, "kS"))
	{
		if (!StringAPI::StartsWith(levelSubstring, "-1") || levelSubstring != "")
		{
			//std::cout << levelSubstring << std::endl;
			std::string decodeBase64 = Encryption::Base64::Decode(levelSubstring, true);
			std::string decodeZLib = Encryption::ZLib::Inflate(decodeBase64, Encryption::ZLib::Format::GZIP);

			if (decodeZLib == "")
			{
				// This means the level is old, decode in ZLIB instead.
				decodeZLib = Encryption::ZLib::Inflate(decodeBase64, Encryption::ZLib::Format::ZLIB);
			}

			ReadLevel(decodeZLib);
		}
		else
		{
			std::cout << "Returned -1" << std::endl;
		}
	}
	else
	{
		ReadLevel(levelSubstring);
	}
}

void GDLevelLoader::ReadLevel(std::string rawData)
{
	std::vector<std::string> allObjects = StringAPI::SplitIntoVector(rawData, ";");
	LoadCustomProperties();

	TextureManager::UploadToGPU();

	for (auto i : allObjects)
	{
		if (!StringAPI::StartsWith(i, "kS"))
		{
			int objId = 0;
			Vector3 position, rotation, scale(1);
			bool fX = false, fY = false, noC = false;
			std::vector<std::string> allValues = StringAPI::SplitIntoVector(i, ",");
			for (int id = 0; id < allValues.size(); id += 2)
			{
				if (allValues[id] != "" && allValues[id + 1] != "")
				{
					int value = std::stoi(allValues[id]);
					float result = 0;
					try
					{
						result = std::stof(allValues[id + 1]);
					}
					catch (...)
					{

					}

					switch (value)
					{
					// Object ID
					case 1:
						objId = result;
						for (auto size : customSize)
						{
							if (size.first == result)
							{
								scale = size.second;
								break;
							}
						}

						for (auto collisions : noCollisions)
						{
							if (collisions == result)
							{
								noC = true;
								break;
							}
						}
						break;

					// X Position
					case 2:
						position.x = result / 30.0f;
						break;

					// Y Position
					case 3:
						position.y = result / 30.0f;
						break;

					//Flip X Texture
					case 4:
						if (allValues[id + 1] == "1")
							fX = true;
						break;

					//Flip Y Texture
					case 5:
						if (allValues[id + 1] == "1")
							fY = true;
						break;

					// Z Rotation
					case 6:
						rotation.z -= result;
						break;

					/*case 21:
						colorID = std::stoi(results[i]);
						break;*/

					// XY Scale
					case 32:
						if (scale.x != 0 && scale.y != 0)
						{
							scale.x *= result;
							scale.y *= result;
						}
						break;
					}
				}
			}
			
			Model* newObj = new Model(Model::Square(), position, rotation, scale);

			newObj->FlipTexture(fX, fY);

			if (!noC)
			{
				BoxCollider* b = newObj->AddScript<BoxCollider>();
				for (auto collSize : customCollisionSize)
				{
					if (collSize.first == objId)
					{
						b->SetScale(collSize.second);
					}
				}
			}

			newObj->SetTexture(*GDTextures[objId]);
			RendererCore::AddModel(*newObj);
		}
	}
}

void GDLevelLoader::LoadCustomProperties()
{
	//Custom Size
	//|
	//|
	//- Rods
	AddCustomSize(15, Vector3(0.21 * 1.5, 1.33 * 1.5, 1));
	AddCustomSize(16, Vector3(0.2 * 1.5, 0.88 * 1.5, 1));
	AddCustomSize(17, Vector3(0.18 * 1.5, 0.41 * 1.5, 1));

	//No Collisions
	//|
	//|
	//- Rods
	AddNoCollisions(15);
	AddNoCollisions(16);
	AddNoCollisions(17);

	//Custom Collision Size
	//|
	//|
	//- Spikes
	AddCustomCollisionSize(8, Vector3(0.2, 0.2, 1));
	AddCustomCollisionSize(39, Vector3(0.2, 0.2, 1));
	AddCustomCollisionSize(103, Vector3(0.2, 0.2, 1));
}

void GDLevelLoader::AddCustomSize(int id, Vector3 size)
{
	customSize.push_back(std::make_pair(id, size));
}

void GDLevelLoader::AddNoCollisions(int id)
{
	noCollisions.push_back(id);
}

void GDLevelLoader::AddCustomCollisionSize(int id, Vector3 size)
{
	customCollisionSize.push_back(std::make_pair(id, size));
}
