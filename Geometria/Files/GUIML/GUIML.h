#include "Graphics/Externals/ImGUIElement.h"
#include "../XML/pugixml.hpp"

struct GUIML
{
	static ImGUIElement* NewGUIML(std::string url, std::string css);
	static ImGUIElement* ReadGUIMLNode(pugi::xml_node node, ImGUIElement* owner);
};