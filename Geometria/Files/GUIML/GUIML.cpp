#include "GUIML.h"
#include "Files\Files.h"
#include "String\StringAPI.h"

std::vector<GUIML::GUIMLCSSStyle> GUIML::_currentCSSStyles;
std::vector<GUIML::GUIMLCSSFont> GUIML::_currentCSSFonts;

Color GUIML::RGBToCol(std::string text)
{
	if (text.find("rgb(") != std::string::npos || text.find("rgba(") != std::string::npos)
	{
		std::string valuesInText = text;
		valuesInText = StringAPI::GetSubstringBetween(valuesInText, "rg", ")");
		valuesInText = StringAPI::RemoveAll(valuesInText, "b(");
		valuesInText = StringAPI::RemoveAll(valuesInText, "ba(");
		valuesInText = StringAPI::RemoveAll(valuesInText, " ");
		valuesInText += ",";
		std::vector<std::string> values = StringAPI::SplitIntoVector(valuesInText, ",");

		Color finalColor;
		int count = 0;
		for (int i = 0; i < values.size(); i++)
		{
			if (values[i] != "")
			{
				if (i == 0)
				{
					finalColor.r = std::stof(values[i]);
					if (finalColor.r > 1)
						finalColor.r /= 255.0f;
				}
				else if (i == 1)
				{
					finalColor.g = std::stof(values[i]);
					if (finalColor.g > 1)
						finalColor.g /= 255.0f;
				}
				else if (i == 2)
				{
					finalColor.b = std::stof(values[i]);
					if (finalColor.b > 1)
						finalColor.b /= 255.0f;
				}
				else if (i == 3)
				{
					finalColor.a = std::stof(values[i]);
					if (finalColor.a > 1)
						finalColor.a /= 255.0f;
				}
				count = i;
			}
		}

		if (count != 3)
			finalColor.a = 1;

		return finalColor;
	}

	return Color();
}

ImGUIElement* GUIML::NewGUIML(std::string url, std::string css)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(url.c_str());

	_currentCSSStyles.clear();
	std::vector<GUIML::GUIMLCSSStyle>().swap(_currentCSSStyles);

	std::cout << css << std::endl;

	std::string cssResult;
	std::vector<std::string> cssLines = Files::ReadAndGetLines(css.c_str());

	for (auto i : cssLines)
	{
		if (i.find(":") != std::string::npos && i.find(";") != std::string::npos)
		{
			std::string getValue = StringAPI::GetSubstringBetween(i, ": ", ";");
			std::string replacedValue = StringAPI::ReplaceAll(getValue, " ", "?");
			std::string result = StringAPI::ReplaceAll(i, getValue, replacedValue);
			i = result;
		}

		i += "\n";
		cssResult += i;
	}

	std::cout << cssResult << std::endl;

	CSSParser prs = CSSParser(cssResult);
	while (!prs.AtEnd()) 
	{
		CSSToken tok = prs.GetCSSToken();

		if (tok.Selector.Type == SelectorType::Class)
		{
			GUIML::GUIMLCSSStyle addClass;

			addClass.className = tok.Selector.Subject;

			for (std::pair<string, CSSStyle> pair : tok.Styles)
			{
				pair.second.Text = StringAPI::ReplaceAll(pair.second.Text, "?", " ");

				// Resolutions
				if (pair.first == "width")
					addClass.width = pair.second.Text;
				else if (pair.first == "height")
					addClass.height = pair.second.Text;

				// Max Resolutions
				else if (pair.first == "max-width")
					addClass.maxWidth = pair.second.Text;
				else if (pair.first == "max-height")
					addClass.maxHeight = pair.second.Text;

				// Min Resolutions
				else if (pair.first == "min-width")
					addClass.minWidth = pair.second.Text;
				else if (pair.first == "min-height")
					addClass.minHeight = pair.second.Text;

				// Align
				else if (pair.first == "align")
					addClass.alignment = pair.second.Text;

				// Font
				else if (pair.first == "font-family")
				{
					if (pair.second.Text.find("\"") != std::string::npos)
						addClass.fontFamily = StringAPI::RemoveAll(pair.second.Text, "\"");
				}
				else if (pair.first == "font-size")
					addClass.fontSize = pair.second.Text;

				// Color
				else if (pair.first == "color" || pair.first == "colour")
					addClass.color = GUIML::RGBToCol(pair.second.Text);
				else if (pair.first == "background-color" || pair.first == "background-colour")
				{
					if (pair.second.Text.find("linear-gradient(") != std::string::npos)
					{
						std::string finalC = pair.second.Text;
						finalC += "!";
						addClass.backgroundGradient = GUIML::TextToGradient(finalC);
					}
					else
						addClass.backgroundColor = GUIML::RGBToCol(pair.second.Text);
				}

				// Border Width
				else if (pair.first == "border-width")
					addClass.borderWidth = pair.second.Text;

				// Border Radius
				else if (pair.first == "border-radius")
					addClass.borderRadius = pair.second.Text;

				// Image
				else if (pair.first == "background-image")
					addClass.backgroundImage = StringAPI::RemoveAll(pair.second.Text, "\"");

				// Padding
				else if (pair.first == "padding-x")
					addClass.paddingX = pair.second.Text;
				else if (pair.first == "padding-y")
					addClass.paddingY = pair.second.Text;

				// Text Wrapping
				else if (pair.first == "wrapping")
					addClass.wrapping = pair.second.Text;

				// Position
				else if (pair.first == "position")
					addClass.positionType = pair.second.Text;

				// Window Title
				else if (pair.first == "enable-title")
					addClass.windowTitle = pair.second.Text;

				// Window Resize
				else if (pair.first == "enable-resize")
					addClass.windowResize = pair.second.Text;

				// [---------------EFFECTS---------------]
				// Box Shadow
				else if (pair.first == "box-shadow-color" || pair.first == "box-shadow-colour")
					addClass.boxShadowColor = RGBToCol(pair.second.Text);

				else if (pair.first == "box-shadow-x")
					addClass.boxShadowX = pair.second.Text;
				else if (pair.first == "box-shadow-y")
					addClass.boxShadowY = pair.second.Text;
			}

			_currentCSSStyles.push_back(addClass);
		}
		else if(tok.Selector.Type == SelectorType::Tag)
		{
			if (tok.Selector.Subject == "@font-face")
			{
				GUIML::GUIMLCSSFont addFont;

				for (std::pair<string, CSSStyle> pair : tok.Styles)
				{
					pair.second.Text = StringAPI::ReplaceAll(pair.second.Text, "?", " ");

					if (pair.first == "font-family")
					{
						if (pair.second.Text.find("\"") != std::string::npos)
							addFont.name = StringAPI::RemoveAll(pair.second.Text, "\"");
					}
					else if (pair.first == "src")
					{
						if (pair.second.Text.find("\"") != std::string::npos)
							addFont.url = StringAPI::RemoveAll(pair.second.Text, "\"");
					}
				}

				_currentCSSFonts.push_back(addFont);
			}
		}
	}

	if (result)
	{
		return ReadGUIMLNode(doc, nullptr);
	}

	return nullptr;
}

ImGUIElement* GUIML::ReadGUIMLNode(pugi::xml_node node, ImGUIElement* owner)
{
	ImGUIElement* Node = nullptr;

	auto guiml = node.child("guiml");

	NodeInfo GUIMLInfo(guiml);

	if (guiml)
	{
		std::string windowType = guiml.attribute("type").as_string();

		if(windowType == "Window")
			Node = new ImGUIElement(ImGUIElement::GUIType::Window, GUIMLInfo.nodeName);
		else if(windowType == "App")
			Node = new ImGUIElement(ImGUIElement::GUIType::AppWindow, GUIMLInfo.nodeName);
		else if(windowType == "")
			Node = new ImGUIElement(ImGUIElement::GUIType::AppWindow, GUIMLInfo.nodeName);

		ApplyCSS(*Node, GUIMLInfo.nodeClass);

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
		auto input = ok.child("input");
		auto div = ok.child("div");
		if (p)
		{
			NodeInfo nodeInfo(p);

			Node = new ImGUIElement(ImGUIElement::GUIType::Text, *owner, p.text().as_string());
			ApplyCSS(*Node, nodeInfo.nodeClass);

			for (pugi::xml_node subnode : p)
				ReadGUIMLNode(subnode, Node);
		}
		else if (cont)
		{
			Node = new ImGUIElement(ImGUIElement::GUIType::SameLine, *owner);
			for (pugi::xml_node subnode : cont)
				ReadGUIMLNode(subnode, Node);
		}
		else if (input)
		{
			NodeInfo nodeInfo(input);

			Node = new ImGUIElement(ImGUIElement::GUIType::InputField, *owner, nodeInfo.nodeName, std::string());
			Node->target = &Node->strData;
			ApplyCSS(*Node, nodeInfo.nodeClass);

			for (pugi::xml_node subnode : input)
				ReadGUIMLNode(subnode, Node);
		}
		else if (div)
		{
			NodeInfo nodeInfo(div);

			Node = new ImGUIElement(ImGUIElement::GUIType::Window, *owner, nodeInfo.nodeName);
			Node->EnableTitle = Node->EnableResize = false;
			ApplyCSS(*Node, nodeInfo.nodeClass);

			for (pugi::xml_node subnode : div)
				ReadGUIMLNode(subnode, Node);
		}

	}

	return Node;
}

void GUIML::ApplyCSS(ImGUIElement& gui, std::string c)
{
	std::vector<GUIMLCSSStyle> guimlCss;
	GUIMLCSSFont getFont;

	for (auto style : _currentCSSStyles)
	{
		if (style.className == c)
		{
			guimlCss.push_back(style);
		}
	}

	for (auto style : guimlCss)
	{
		for (auto font : _currentCSSFonts)
		{
			if (font.name == style.fontFamily)
			{
				getFont = font;
			}
		}
	}

	for (auto css : guimlCss)
	{
		if (css.width != "")
			CSS_ApplyResolution(gui, "width", css.width);
		if (css.height != "")
			CSS_ApplyResolution(gui, "height", css.height);
		if (css.alignment != "")
			CSS_ApplyAlignment(gui, css.alignment);

		if (css.fontSize != "")
			gui.fontSize = PixelsToFloat(css.fontSize);
		if (css.fontFamily != "")
			CSS_ApplyFont(gui, getFont);

		if (css.color != Color(-1, -1, -1, -1))
			CSS_ApplyColor(gui, css.color);
		if (css.backgroundColor != Color(-1, -1, -1, -1))
			CSS_ApplyBackgroundColor(gui, css.backgroundColor);
		if (css.backgroundGradient.size() != 0)
			gui.AddBackgroundGradient(css.backgroundGradient);

		if (css.borderWidth != "")
			CSS_ApplyBorderWidth(gui, css.borderWidth);
		if (css.borderRadius != "")
			CSS_ApplyBorderRadius(gui, css.borderRadius);

		if (css.maxWidth != "")
			CSS_ApplyMaxOrMinResolution(gui, "max-width", css.maxWidth);
		if (css.maxHeight != "")
			CSS_ApplyMaxOrMinResolution(gui, "max-height", css.maxHeight);
		if (css.minWidth != "")
			CSS_ApplyMaxOrMinResolution(gui, "min-width", css.minWidth);
		if (css.minHeight != "")
			CSS_ApplyMaxOrMinResolution(gui, "min-height", css.minHeight);

		if (css.backgroundImage != "")
			CSS_ApplyBackgroundImage(gui, css.backgroundImage);

		if (css.paddingX != "")
			gui.padding.x = PixelsToFloat(css.paddingX);
		if (css.paddingY != "")
			gui.padding.y = PixelsToFloat(css.paddingY);

		if (css.wrapping != "")
			gui.wrapped = TextToBool(css.wrapping);

		if (css.positionType != "")
			CSS_ApplyPositionType(gui, css.positionType);

		if (css.windowTitle != "")
			gui.EnableTitle = TextToBool(css.windowTitle);

		if (css.windowResize != "")
			gui.EnableResize = TextToBool(css.windowResize);

		float bSX = 0, bSY = 0;
		if (css.boxShadowX != "")
			bSX = PixelsToFloat(css.boxShadowX);
		if (css.boxShadowY != "")
			bSY = PixelsToFloat(css.boxShadowY);

		if (css.boxShadowColor != Color(-1, -1, -1, -1))
			gui.AddBoxShadow(css.boxShadowColor, Vector2(bSX, bSY));
	}
}

void GUIML::CSS_ApplyResolution(ImGUIElement& gui, std::string var, std::string value)
{
	int finalW = -1, finalH = -1;
	bool screen = false;
	if (var == "width")
	{
		if (value.find("px") != std::string::npos)
		{
			float w = PixelsToFloat(value);
			gui.size.x = w;
			screen = false;
		}
		else if (value.find("vw") != std::string::npos)
		{
			screen = true;
			std::string res = StringAPI::ReplaceAll(value, "vw", "");
			finalW = std::stoi(res);
			gui.screenSize.x = finalW;
		}
	}

	if (var == "height")
	{
		if (value.find("px") != std::string::npos)
		{
			float h = PixelsToFloat(value);
			gui.size.y = h;
			screen = false;
		}
		else if (value.find("vh") != std::string::npos)
		{
			screen = true;
			std::string res = StringAPI::ReplaceAll(value, "vh", "");
			finalH = std::stoi(res);
			gui.screenSize.y = finalH;
		}
	}

	if (screen)
	{
		std::cout << "Screen set to true!" << '\n';
		gui.ScaleWithScreenResolution(gui.screenSize);
	}
	else
		gui.Scale(gui.size);
}

void GUIML::CSS_ApplyMaxOrMinResolution(ImGUIElement& gui, std::string var, std::string value)
{
	float finalW = -1, finalH = -1;
	if (var == "max-width")
	{
		finalW = PixelsToFloat(value);
		gui.maxScale.x = finalW;
	}

	if (var == "max-height")
	{
		finalH = PixelsToFloat(value);
		gui.maxScale.y = finalH;
	}

	if (var == "min-width")
	{
		finalW = PixelsToFloat(value);
		gui.minScale.x = finalW;
	}

	if (var == "min-height")
	{
		finalH = PixelsToFloat(value);
		gui.minScale.y = finalH;
	}
}

void GUIML::CSS_ApplyAlignment(ImGUIElement& gui, std::string value)
{
	if (value.find("%") == std::string::npos)
	{
		if (value == "top-left")
			gui.Alignment = ImGUIElement::AlignTo::TopLeft;
		else if (value == "top")
			gui.Alignment = ImGUIElement::AlignTo::Top;

		else if (value == "left")
			gui.Alignment = ImGUIElement::AlignTo::Left;
		else if (value == "center")
			gui.Alignment = ImGUIElement::AlignTo::Center;
	}
	else
	{
		std::vector<std::string> values = StringAPI::SplitIntoVector(value, " ");
		if (values.size() == 2)
		{
			float x = std::stof(StringAPI::RemoveAll(values[0], "%"));
			x /= 100.0f;

			float y = std::stof(StringAPI::RemoveAll(values[1], "%"));
			y /= 100.0f;

			gui.SetAlignPivot(Vector2(x, y));
		}
	}
}

void GUIML::CSS_ApplyFont(ImGUIElement& gui, GUIMLCSSFont f)
{
	ImGUIEFont* font = ImGUIElement::GetFont(f.name.c_str(), gui.fontSize);

	if (font != nullptr)
		gui.font = font;
	else if (font == nullptr)
	{
		font = ImGUIElement::AddFont(f.name.c_str(), f.url.c_str(), gui.fontSize);
		gui.font = font;
	}
}

void GUIML::CSS_ApplyColor(ImGUIElement& gui, Color col)
{
	gui.SetFontColor(col);
}

void GUIML::CSS_ApplyBackgroundColor(ImGUIElement& gui, Color col)
{
	gui.SetColor(col);
}

void GUIML::CSS_ApplyBorderWidth(ImGUIElement& gui, std::string width)
{
	gui.borderSize = PixelsToFloat(width);
}

void GUIML::CSS_ApplyBorderRadius(ImGUIElement& gui, std::string radius)
{
	gui.borderRadius = PixelsToFloat(radius);
}

void GUIML::CSS_ApplyBackgroundImage(ImGUIElement& gui, std::string url)
{
	gui.backgroundImage = new Texture(url.c_str(), Texture::Type::Default);
}

void GUIML::CSS_ApplyPositionType(ImGUIElement& gui, std::string type)
{
	if (type == "relative")
		gui.itemPos = ImGUIElement::ItemPosition::Relative;
	else if (type == "absolute")
		gui.itemPos = ImGUIElement::ItemPosition::Absolute;
}

std::vector<std::pair<Color, float>> GUIML::TextToGradient(std::string content)
{
	std::string valuesInString = StringAPI::GetSubstringBetween(content, "linear-gradient(", ")!");
	std::vector<std::string> allValues = StringAPI::SplitIntoVector(content, "%,");
	std::vector<std::pair<Color, float>> v;

	for (auto i : allValues)
	{
		// TODO: Add Degree System
		if (i.find("deg") == std::string::npos)
		{
			i += "%";
			std::string percentage = StringAPI::GetSubstringBetween(i, ") ", "%");
			float finalPercentage = std::stof(percentage);

			std::string percentageText = " ";
			percentageText += percentage;
			percentageText += "%";

			std::string removePercentageFromCS = StringAPI::RemoveAll(i, percentageText);
			std::string colorString = StringAPI::RemoveAll(removePercentageFromCS, "linear-gradient(");

			Color finalColor = RGBToCol(colorString);
			v.push_back(std::make_pair(finalColor, finalPercentage));
		}
	}

	return v;
}

float GUIML::PixelsToFloat(std::string val)
{
	if (val.find("px") != std::string::npos)
	{
		std::string v = StringAPI::ReplaceAll(val, "px", "");
		int pix = std::stoi(v);
		return pix;
	}

	return 0.0f;
}

bool GUIML::TextToBool(std::string val)
{
	if (val == "true")
		return true;
	else if (val == "false")
		return false;

	return false;
}
