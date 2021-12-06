#include "GDLevelLoader.h"
#include "../../../Encryption/Encryption.h"
#include "../../../String/StringAPI.h"
#include "../../../Web/Web.h"
#include "../../../Physics/Colliders/BoxCollider.h"

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

	std::string urlResult = Web::Post("http://www.boomlings.com/database/downloadGJLevel22.php", "gameVersion=20&binaryVersion=35&gdw=0&levelID=1556066&secret=Wmfd2893gb7");

	std::string levelSubstring = StringAPI::GetSubstringBetween(urlResult, ":4:", ":5:");

	if (!StringAPI::StartsWith(levelSubstring, "kS"))
	{
		if (!StringAPI::StartsWith(levelSubstring, "-1") || levelSubstring != "")
		{
			std::cout << levelSubstring << std::endl;
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
	for (auto i : allObjects)
	{
		if (!StringAPI::StartsWith(i, "kS"))
		{
			Vector3 position, rotation, scale(1);
			std::vector<std::string> allValues = StringAPI::SplitIntoVector(i, ",");
			for (int i = 0; i < allValues.size(); i += 2)
			{
				int value = std::stoi(allValues[i]);
				switch (value)
				{
				case 2:
					position.x = std::stof(allValues[i + 1]) / 30.0f;
				case 3:
					position.y = std::stof(allValues[i + 1]) / 30.0f;
				}
			}

			Model* newObj = new Model(Model::Square(), position, rotation, scale);
			newObj->AddScript<BoxCollider>();
			RendererCore::AddModel(*newObj);
		}
	}
}
