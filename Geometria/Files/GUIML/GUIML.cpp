#include "GUIML.h"

ImGUIElement* GUIML::NewGUIML(std::string url, std::string css)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("Files/GUIML/Tests/index.guiml");

	return ReadGUIMLNode(doc, nullptr);
}

ImGUIElement* GUIML::ReadGUIMLNode(pugi::xml_node node, ImGUIElement* owner)
{
	ImGUIElement* Node = nullptr;

	auto guiml = node.child("guiml");

	if (guiml)
	{
		std::string windowType = guiml.attribute("type").as_string();

		if(windowType == "Window")
			Node = new ImGUIElement(ImGUIElement::GUIType::Window, guiml.attribute("name").as_string());
		else if(windowType == "App")
			Node = new ImGUIElement(ImGUIElement::GUIType::AppWindow, guiml.attribute("name").as_string());
		else if(windowType == "")
			Node = new ImGUIElement(ImGUIElement::GUIType::AppWindow, guiml.attribute("name").as_string());
		
		for (pugi::xml_node subnode : guiml)
			ReadGUIMLNode(subnode, Node);
	}

	if (owner != nullptr)
	{
		std::ostringstream content;
		node.print(content);

		pugi::xml_document ok;
		pugi::xml_parse_result result = ok.load_string(content.str().c_str());

		auto p = ok.child("p");
		auto cont = ok.child("cont");
		if (p)
		{
			Node = new ImGUIElement(ImGUIElement::GUIType::Text, *owner, p.text().as_string());
			for (pugi::xml_node subnode : p)
				ReadGUIMLNode(subnode, Node);
		}
		else if (cont)
		{
			Node = new ImGUIElement(ImGUIElement::GUIType::SameLine, *owner);
			for (pugi::xml_node subnode : p)
				ReadGUIMLNode(subnode, Node);
		}

	}

	return Node;
}
