#include "GDLevelLoader.h"
#include "../../../Encryption/Encryption.h"
#include "../../../String/StringAPI.h"

void GDLevelLoader::OnStart()
{
	std::string result = Web::Post("http://www.boomlings.com/database/downloadGJLevel22.php", "gameVersion=20&binaryVersion=35&gdw=0&levelID=1650666&secret=Wmfd2893gb7");
	std::string levelSubstring = StringAPI::GetSubstringBetween(result, ":4:", ":5:");

	if (!StringAPI::StartsWith(levelSubstring, "kS"))
	{
		std::string decodeBase64 = Encryption::Base64::Decode(levelSubstring, true);
		std::string decodeZLib = Encryption::ZLib::Inflate(decodeBase64, Encryption::ZLib::Format::GZIP);

		ReadLevel(decodeZLib);
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
			RendererCore::AddModel(*newObj);
		}
	}
}
