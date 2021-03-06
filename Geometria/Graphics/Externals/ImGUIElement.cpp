#include "ImGUIElement.h"
#include "../Cores/Renderer/RendererCore.h"
#include "../Externals/SceneAndDrawCall.h"
#include <experimental/filesystem>
#include "../Cores/MainAPI/Graphics.h"
#include "../../Input/Input.h"

bool ImGUIElement::_isMouseOnAnyWindow = false;
std::vector<ImGUIEFont*> ImGUIElement::allFonts;

void ImGUIElement::LoadElements()
{
	
}

ImGUIEFont* ImGUIElement::AddFont(const char* name, const char* url, int size)
{
	ImGUIEFont* font = new ImGUIEFont(name, url, size);
	allFonts.push_back(font);
	return font;
}

ImGUIEFont* ImGUIElement::GetFont(const char* nameOrUrl, int size)
{
	bool isUrl = false;
	std::string nu = nameOrUrl;
	if (nu.find("/") != std::string::npos)
		isUrl = true;

	for (auto i : allFonts)
	{
		if (isUrl)
		{
			if (i->size == size && i->url == nu)
				return i;
		}
		else
		{
			if (i->size == size && i->name == nu)
				return i;
		}
	}

	return nullptr;
}

void ImGUIElement::Move(Vector2 position)
{
	moveToPosition = position;
	_requestForceMove = true;
}

void ImGUIElement::Scale(Vector2 scale)
{
	scaleTo = scale;
	size = scale;
	_requestForceScale = true;
}

void ImGUIElement::ScaleWithScreenResolution(Vector2 percentageScale)
{
	screenSize = percentageScale;
	int finalW = 0, finalH = 0;

	if (screenSize.x == -1)
		finalW = size.x;
	else
		finalW = Graphics::GetMainWindow().width * (screenSize.x / 100.0f);

	if (minScale.x != -1)
		if (finalW < minScale.x)
			finalW = minScale.x;

	if (maxScale.x != -1)
		if (finalW > maxScale.x)
			finalW = maxScale.x;


	if (screenSize.y == -1)
	{
		finalH = size.y;
	}
	else
		finalH = Graphics::GetMainWindow().height * (screenSize.y / 100.0f);

	if (minScale.y != -1)
		if (finalH < minScale.y)
			finalH = minScale.y;

	if (maxScale.y != -1)
		if (finalH > maxScale.y)
			finalH = maxScale.y;


	Scale(Vector2(finalW, finalH));
}

bool ImGUIElement::IsMouseOnAnyWindow()
{
	return _isMouseOnAnyWindow;
}

void ImGUIElement::SetAlignPivot(Vector2 a)
{
	Alignment = AlignTo::Custom;
	alignPivot = a;
}

void ImGUIElement::OpenWithMouseButton(int input)
{
	mouseInput = input;
}

void ImGUIElement::SetColor(Color col)
{
	isColored = true;
	colorRef = new Color(col);
}

void ImGUIElement::SetFontColor(Color col)
{
	isColored = true;

	if (guiType == GUIType::Window)
		fontColor = col;
	else
		colorRef = new Color(col);
}

void ImGUIElement::SetMinScale(Vector2 min)
{
	minScale = min;
}

void ImGUIElement::SetMaxScale(Vector2 max)
{
	maxScale = max;
}

void ImGUIElement::AddBackgroundGradient(std::vector<std::pair<Color, float>> bg)
{
	RemoveBackgroundGradient();
	backgroundGradients = bg;
}

void ImGUIElement::RemoveBackgroundGradient()
{
	backgroundGradients.clear();
	std::vector<std::pair<Color, float>>().swap(backgroundGradients);
}

void ShadeVertsLinearColorGradientSetAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = ImVec2(gradient_p1.x - gradient_p0.x, gradient_p1.y - gradient_p0.y);
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    const int col0_r = (int)(col0 >> IM_COL32_R_SHIFT) & 0xFF;
    const int col0_g = (int)(col0 >> IM_COL32_G_SHIFT) & 0xFF;
    const int col0_b = (int)(col0 >> IM_COL32_B_SHIFT) & 0xFF;
    const int col0_a = (int)(col0 >> IM_COL32_A_SHIFT) & 0xFF;
    const int col_delta_r = ((int)(col1 >> IM_COL32_R_SHIFT) & 0xFF) - col0_r;
    const int col_delta_g = ((int)(col1 >> IM_COL32_G_SHIFT) & 0xFF) - col0_g;
    const int col_delta_b = ((int)(col1 >> IM_COL32_B_SHIFT) & 0xFF) - col0_b;
    const int col_delta_a = ((int)(col1 >> IM_COL32_A_SHIFT) & 0xFF) - col0_a;
    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(ImVec2(vert->pos.x - gradient_p0.x, vert->pos.y - gradient_p0.y), gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
        int r = (int)(col0_r + col_delta_r * t);
        int g = (int)(col0_g + col_delta_g * t);
        int b = (int)(col0_b + col_delta_b * t);
        int a = (int)(col0_a + col_delta_a * t);
        vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (a << IM_COL32_A_SHIFT);
    }
}

void ImGUIElement::RenderGradients(std::vector<std::pair<Color, float>>& b, ImVec2 topL, ImVec2 botR)
{
	std::vector<std::pair<Color, float>> finalB;

	if (b[0].second != 0)
		finalB.push_back(std::make_pair(b[0].first, 0));

	for (auto i : b)
		finalB.push_back(i);

	if(b[b.size() - 1].second != 100)
		finalB.push_back(std::make_pair(b[b.size() - 1].first, 100));

	for (int i = 1; i < finalB.size(); i++)
	{
		ImVec2 topLFinal = topL, botRFinal = botR;

		topLFinal.y = (ImGui::GetWindowSize().y * finalB[i - 1].second / 100) + topL.y;
		botRFinal.y = (ImGui::GetWindowSize().y * finalB[i].second / 100) + topL.y;

		/*std::cout << "Top Left: " << topLFinal.x << " x " << topLFinal.y << " || Percentage: " << finalB[i - 1].second << '\n';
		std::cout << "Bottom Right: " << botRFinal.x << " x " << botRFinal.y << " || Percentage: " << finalB[i].second << '\n';
		std::cout << "--------------------------------" << '\n';*/

		ImColor col2(finalB[i].first.r, finalB[i].first.g, finalB[i].first.b, finalB[i].first.a);
		ImColor col1(finalB[i - 1].first.r, finalB[i - 1].first.g, finalB[i - 1].first.b, finalB[i - 1].first.a);

		/*ImGui::GetWindowDrawList()->AddRectFilledMultiColor(topLFinal, botRFinal,
			col1, col1, col2, col2);*/

		float finalBorderRadius = 0;
		ImDrawFlags flags;
		if (i == 1)
		{
			if (finalB.size() == 2)
				flags = ImDrawCornerFlags_All;
			else
				flags = ImDrawCornerFlags_Top;

			finalBorderRadius = borderRadius;
		}
		else if (i == finalB.size() - 1)
		{
			flags = ImDrawCornerFlags_Bot;
			finalBorderRadius = borderRadius;
		}

		const int vtx_idx_0 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
		ImGui::GetWindowDrawList()->AddRectFilled(topLFinal, botRFinal, ImColor(1, 1, 1, 1), finalBorderRadius, flags);
		const int vtx_idx_1 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
		ImGui::GetWindowDrawList()->AddRectFilled(topLFinal, botRFinal, ImColor(1, 1, 1, 1), finalBorderRadius, flags);
		const int vtx_idx_2 = ImGui::GetWindowDrawList()->VtxBuffer.Size;

		ShadeVertsLinearColorGradientSetAlpha(ImGui::GetWindowDrawList(), vtx_idx_0, vtx_idx_1, topLFinal, ImVec2(botRFinal.x, topLFinal.y), col1, col2);
		ShadeVertsLinearColorGradientSetAlpha(ImGui::GetWindowDrawList(), vtx_idx_1, vtx_idx_2, topLFinal, ImVec2(topLFinal.x, botRFinal.y), col1, col2);
	}
}

void ImGUIElement::RenderBoxShadow()
{
	Vector2 lastPosPlusOffset = lastPosition + boxShadow->offset;
	Vector2 lastPosPlusSize = lastPosPlusOffset + lastSize;

	ImDrawList* draw_list = nullptr;
	if (owner != nullptr)
		draw_list = ImGui::GetWindowDrawList();
	else
		draw_list = ImGui::GetBackgroundDrawList();

	draw_list->AddRectFilled(ImVec2(lastPosPlusOffset.x, lastPosPlusOffset.y),
		ImVec2(lastPosPlusSize.x, lastPosPlusSize.y),
		ImColor(boxShadow->col.r, boxShadow->col.g, boxShadow->col.b, boxShadow->col.a),
		borderRadius);
}

ImGUIEBoxShadow* ImGUIElement::AddBoxShadow(Color shadowCol, Vector2 offset)
{
	if (boxShadow == nullptr)
		boxShadow = new ImGUIEBoxShadow();

	boxShadow->col = shadowCol;
	boxShadow->offset = offset;

	return boxShadow;
}

ImGUIEBoxShadow* ImGUIElement::RemoveBoxShadow()
{
	if (boxShadow != nullptr)
	{
		delete boxShadow;
		boxShadow = nullptr;
	}
	return nullptr;
}

void ImGUIElement::Delete()
{
	onClickEvents.clear();
	onChangeEvents.clear();
	onFileChangeEvents.clear();
	onUIUpdateBeginEvents.clear();
	onUIUpdateEndEvents.clear();

	for (int i = 0; i < allElements.size(); i++)
	{
		allElements[i]->Delete();

		for (int h = 0; h < Hierarchy::allScripts.size();)
		{
			if (Hierarchy::allScripts[h]->scriptId == allElements[i]->scriptId)
			{
				Hierarchy::allScripts.erase(Hierarchy::allScripts.begin() + h);
			}
			else
			{
				h++;
			}
		}
	}
	//std::vector<ScriptBehaviour*>().swap(Hierarchy::allScripts);
	allElements.clear();
}

void ImGUIElement::OnBegin()
{

}

ImGUIElement::ImGUIElement() : guiType(GUIType::None) {}
ImGUIElement::ImGUIElement(GUIType gT) : guiType(gT) {}
ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner) : guiType(gT) 
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}
ImGUIElement::ImGUIElement(GUIType gT, std::string t) : guiType(gT), text(t) {}
ImGUIElement::ImGUIElement(GUIType gT, int value) : guiType(gT), val(value) {}
ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, int value) : guiType(gT), val(value)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}
ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, float* floatR) : guiType(gT), fRef(floatR)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}
ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, float* floatR, float min, float max) : guiType(gT), fRef(floatR), minVal(min), maxVal(max)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}
ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, std::string t) : guiType(gT), text(t) 
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}

ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, std::string t, int* iR) : guiType(gT), text(t), iRef(iR)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}

ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, std::string t, std::string t2) : guiType(gT), text(t), secondText(t2)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}

ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, std::string t, Vector4* v4r) : guiType(gT), text(t), v4ref(v4r) 
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}

ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, std::string t, std::string* targ) : guiType(gT), text(t), target(targ)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}

ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, std::string t, Color* colRef) : guiType(gT), text(t), colorRef(colRef)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}

ImGUIElement::ImGUIElement(GUIType gT, ImGUIElement& owner, ScriptBehaviour* sr) : guiType(gT), scriptRef(sr)
{
	owner.allElements.push_back(this);
	this->owner = &owner;
}

float vectorToFloat[3] = { 0, 0, 0 };
void ImGUIElement::OnUpdate()
{
	if (isEnabled)
	{
		if (mouseInput != -1)
		{
			if (Input::GetMouseButton(mouseInput))
			{
				isOpen = true;
			}
		}

		for (int i = 0; i < onUIUpdateBeginEvents.size(); i++)
		{
			onUIUpdateBeginEvents[i]();
		}


		bool currentWrapping = false;
		if (owner != nullptr)
		{
			if (owner->wrapped)
				currentWrapping = true;
			else
				currentWrapping = wrapped;
		}
		else
		{
			currentWrapping = wrapped;
		}

		if (guiType != GUIType::Window)
		{
			switch (Alignment)
			{
			case Center:
				float font_size = ImGui::CalcTextSize(text.c_str()).x;
				float text_indentation = (ImGui::GetWindowSize().x - font_size) * 0.5f;
				float min_indentation = 20.0f;
				if (text_indentation <= min_indentation) {
					text_indentation = min_indentation;
				}
				
				ImGui::SetCursorPosX(text_indentation);

				if (currentWrapping)
					ImGui::PushTextWrapPos(ImGui::GetWindowSize().x - text_indentation);
				break;
			}
		}

		ImGuiStyle& style = ImGui::GetStyle();
		ImColor styleCol = style.Colors[ImGuiCol_Text];
		ImColor windowBgCol = style.Colors[ImGuiCol_WindowBg];
		ImVec2 padding = style.WindowPadding;

		float winBorderSize = style.WindowBorderSize;
		float childWinBorderSize = style.ChildBorderSize;

		style.WindowBorderSize = borderSize;
		style.ChildBorderSize = borderSize;

		float winBorderRadius = style.WindowRounding,
			childWinBorderRadius = style.ChildRounding,
			frameBorderRadius = style.FrameRounding;

		style.WindowRounding = borderRadius;
		style.ChildRounding = borderRadius;
		style.FrameRounding = borderRadius;

		style.WindowPadding = ImVec2(this->padding.x, this->padding.y);

		if (isColored && guiType != GUIType::Window)
		{
			if (colorRef != nullptr)
			{
				style.Colors[ImGuiCol_Text] = ImColor(colorRef->r, colorRef->g, colorRef->b, colorRef->a);
			}
		}

		if (UITag == "")
			textFinal = text;
		else
			textFinal = text + "###" + UITag;

		if (font != nullptr)
		{
			ImGui::PushFont(font->font);
		}

		ImGuiWindowFlags window_flags = 0;

		if (lastPosition != Vector2(-1, -1) && lastSize != Vector2(-1, -1))
		{
			if (boxShadow != nullptr)
				RenderBoxShadow();
		}

		float alignW = 0, alignH = 0;
		switch (guiType)
		{
		case GUIType::Window:
			if (screenSize.x != -1 || screenSize.y != -1)
				ScaleWithScreenResolution(screenSize);

			if (isColored)
			{
				if (backgroundImage == nullptr)
				{
					if (colorRef != nullptr)
					{
						style.Colors[ImGuiCol_WindowBg] = ImColor(colorRef->r, colorRef->g, colorRef->b, colorRef->a);
					}
				}

				if (fontColor != Color(-1, -1, -1, -1))
				{
					style.Colors[ImGuiCol_Text] = ImColor(fontColor.r, fontColor.g, fontColor.b, fontColor.a);
				}
			}

			if (backgroundImage != nullptr || backgroundGradients.size() != 0)
				style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 0);

			switch (Alignment)
			{
			case TopLeft:
				alignPivot = Vector2(0, 0);
				break;
			case Top:
				alignPivot = Vector2(0.5, 0);
				break;
			case TopRight:
				alignPivot = Vector2(1, 0);

			case Left:
				alignPivot = Vector2(0, 0.5);
				break;
			case Center:
				alignPivot = Vector2(0.5, 0.5);
				break;
			case Right:
				alignPivot = Vector2(1, 0.5);

			case BottomLeft:
				alignPivot = Vector2(0, 1);
				break;
			case Bottom:
				alignPivot = Vector2(0.5, 1);
				break;
			case BottomRight:
				alignPivot = Vector2(1, 1);
			}

			if (owner != nullptr)
			{
				alignW = (ImGui::GetWindowPos().x + ((ImGui::GetWindowSize().x - size.x) * alignPivot.x));
				alignH = (ImGui::GetWindowPos().y + ((ImGui::GetWindowSize().y - size.y) * alignPivot.y));
			}
			else
			{
				alignW = Graphics::GetMainWindow().width * alignPivot.x - (size.x / 2);
				alignH = Graphics::GetMainWindow().height * alignPivot.y - (size.y / 2);
			}

			if (alignPivot != Vector2(-1, -1))
			{
				Move(Vector2(
					alignW,
					alignH
				));
			}

			bool window;

			if (!EnableResize) window_flags |= ImGuiWindowFlags_NoResize;
			if (!EnableTitle) window_flags |= ImGuiWindowFlags_NoTitleBar;
			if (!EnableScrolling) window_flags |= ImGuiWindowFlags_NoScrollbar;
			if (!CanBeMoved) window_flags |= ImGuiWindowFlags_NoMove;
			if (containsWindowsInside) { window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus; containsWindowsInside = false; }

			if (!SaveInFile)
				ImGui::GetIO().IniFilename = NULL;

			if (isOpen)
			{
				if (isOpen != _lastOpenState)
				{
					for (int i = 0; i < onOpenEvents.size(); i++)
					{
						onOpenEvents[i]();
					}
					_lastOpenState = isOpen;
				}

				bool isAbsolute = owner != nullptr && itemPos == ItemPosition::Absolute;

				if (_requestForceMove)
				{
					ImGui::SetNextWindowPos(ImVec2(moveToPosition.x, moveToPosition.y), ImGuiCond_Always);
					_requestForceMove = false;
				}

				if (_requestForceScale)
				{
					ImGui::SetNextWindowSize(ImVec2(scaleTo.x, scaleTo.y), ImGuiCond_Always);
					_requestForceScale = false;
				}

				if (owner == nullptr || isAbsolute)
				{
					if (isAbsolute)
					{
						owner->containsWindowsInside = true;
					}

					if (!enableOpenAndClose)
					{
						window = ImGui::Begin(textFinal.c_str(), nullptr, window_flags);
					}
					else
					{
						window = ImGui::Begin(textFinal.c_str(), &isOpen, window_flags);
					}
				}
				else
				{
					window = ImGui::BeginChild(textFinal.c_str(), ImVec2(size.x, size.y), true);
					owner->containsWindowsInside = true;
				}

				ImGUIElement::_isMouseOnAnyWindow = false;

				if (window)
				{
					if (ImGui::IsWindowHovered())
						ImGUIElement::_isMouseOnAnyWindow = true;

					ImVec2 window_TopLeft(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
					ImVec2 window_BottomRight(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);

					if (owner != nullptr && colorRef != nullptr)
					{
						ImGui::GetWindowDrawList()->AddRectFilled(window_TopLeft, window_BottomRight, ImColor(style.Colors[ImGuiCol_WindowBg]), borderRadius);
					}

					if (backgroundGradients.size() != 0)
					{
						ImGUIElement::RenderGradients(backgroundGradients, window_TopLeft, window_BottomRight);
					}

					if (backgroundImage != nullptr)
					{
						if (backgroundImage->IsLoadedToGPU())
						{
							ImVec2 winPad = style.WindowPadding;

							ImVec2 uvMin(float(backgroundImage->finalRect.x) / float(TextureManager::textureGroups[backgroundImage->texGroupId].width),
								float(backgroundImage->finalRect.y) / float(TextureManager::textureGroups[backgroundImage->texGroupId].height));

							ImVec2 uvMax(float(backgroundImage->finalRect.x + backgroundImage->finalRect.width) / float(TextureManager::textureGroups[backgroundImage->texGroupId].width),
								float(backgroundImage->finalRect.y + backgroundImage->finalRect.height) / float(TextureManager::textureGroups[backgroundImage->texGroupId].height));

							ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)TextureManager::textureGroups[backgroundImage->texGroupId].texture,
								window_TopLeft,
								window_BottomRight, uvMin, uvMax);
						}
					}

					for (int i = 0; i < allElements.size(); i++)
					{
						allElements[i]->OnUpdate();
					}
				}

				lastPosition = Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
				lastSize = Vector2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

				if (owner == nullptr)
					ImGui::End();
				else
					ImGui::EndChild();

				if (isAbsolute)
				{
					ImGui::SetCursorScreenPos(ImVec2(ImGui::GetWindowPos().x + padding.x, ImGui::GetWindowPos().y + ImGui::GetFontSize() + padding.y));
				}
			}
			else
			{
				_lastOpenState = false;
			}

			if (!SaveInFile)
				ImGui::GetIO().IniFilename = "imgui.ini";
			break;

		case GUIType::AppWindow:
			guiType = Window;
			EnableResize = EnableScrolling = EnableTitle = CanBeMoved = SaveInFile = false;
			break;

		case GUIType::MainMenuBar:
			if (ImGui::BeginMainMenuBar())
			{
				style.Colors[ImGuiCol_Text] = styleCol;
				for (int i = 0; i < allElements.size(); i++)
				{
					allElements[i]->OnUpdate();
				}

				ImGui::EndMainMenuBar();
			}
			break;

		case GUIType::Button:
			if (ImGui::Button(textFinal.c_str(), ImVec2(size.x, size.y)))
			{
				for (int i = 0; i < onClickEvents.size(); i++)
				{
					onClickEvents[i]();
				}
			}
			break;

		case GUIType::Menu:
			if (ImGui::BeginMenu(textFinal.c_str()))
			{
				style.Colors[ImGuiCol_Text] = styleCol;
				for (int i = 0; i < allElements.size(); i++)
				{
					allElements[i]->OnUpdate();
				}
				ImGui::EndMenu();
			}
			break;

		case GUIType::MenuItem:
			if (ImGui::MenuItem(textFinal.c_str()))
			{
				style.Colors[ImGuiCol_Text] = styleCol;
				for (int i = 0; i < allElements.size(); i++)
				{
					allElements[i]->isOpen = true;
				}
			}
			break;

		case GUIType::ListButton:
			if (allElements.size() == 0)
			{
				if (ImGui::Selectable(textFinal.c_str(), &clicked))
				{
					for (int i = 0; i < onClickEvents.size(); i++)
					{
						onClickEvents[i]();
					}
				}
			}
			else
			{
				ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth;

				if (clicked)
				{
					base_flags |= ImGuiTreeNodeFlags_Selected;
				}

				if (ImGui::TreeNodeEx(&UITag, base_flags, text.c_str()))
				{
					if (ImGui::IsItemClicked())
					{
						clicked = true;
					}

					if (clicked)
					{
						for (int i = 0; i < onClickEvents.size(); i++)
						{
							onClickEvents[i]();
						}
					}

					for (int i = 0; i < allElements.size(); i++)
					{
						allElements[i]->OnUpdate();
					}
					ImGui::TreePop();
				}
			}
			break;

		case GUIType::DragVector3:
			ImGui::DragFloat3(textFinal.c_str(), (float*)v4ref, 0.01f);
			v4ref->SendChangeEvent();
			break;

		case GUIType::DragInt:
			ImGui::DragInt(textFinal.c_str(), iRef, 0.1f);
			break;

		case GUIType::InputField:
			if (ImGUIElement::InputText(textFinal.c_str(), target, ImGuiInputTextFlags_CallbackResize, InputTextCallback, (void*)target))
			{
				for (int i = 0; i < onChangeEvents.size(); i++)
				{
					onChangeEvents[i]();
				}
			}
			break;

		case GUIType::Text:
			if (!currentWrapping)
				ImGui::Text(textFinal.c_str());
			else
			{
				ImGui::TextWrapped(textFinal.c_str());
				if(Alignment == AlignTo::Center)
					ImGui::PopTextWrapPos();
			}
			break;

		case GUIType::ColorEditorButtonRGBA:
			ImGui::ColorEdit4(textFinal.c_str(), (float*)colorRef);
			break;

		case GUIType::ColorEditorButtonRGB:
			ImGui::ColorEdit3(textFinal.c_str(), (float*)colorRef);
			break;

		case GUIType::Columns:

			if (ColumnProperties.DynamicResizing)
			{
				if (ColumnProperties.size == nullptr)
					ColumnProperties.size = new float(256);

				float padding = 16.0f,
					cellSize = *ColumnProperties.size + padding,
					panelWidth = ImGui::GetContentRegionAvail().x;

				int columnCount = (int)(panelWidth / cellSize);

				if (columnCount < 1)
					columnCount = 1;

				val = columnCount;
			}

			ImGui::Columns(val, 0, ColumnProperties.CanResizeManually);
			break;

		case GUIType::EndColumns:
			ImGui::Columns(1);
			break;

		case GUIType::NextColumn:
			ImGui::NextColumn();
			break;

		case GUIType::ImGUIDemoWindow:
			ImGui::ShowDemoWindow();
			break;

		case GUIType::SliderFloat:
			if (ImGui::SliderFloat(textFinal.c_str(), fRef, minVal, maxVal))
			{
				for (int i = 0; i < onChangeEvents.size(); i++)
				{
					onChangeEvents[i]();
				}
			}
			break;

		case GUIType::FileSearch:
			if (ImGui::Button(textFinal.c_str(), ImVec2(size.x, size.y)))
			{
				for (int i = 0; i < onClickEvents.size(); i++)
				{
					onClickEvents[i]();
				}
			}

			ImGui::SameLine();

			if (storedPath != "")
			{
				ImGui::Text(std::experimental::filesystem::path(storedPath).filename().u8string().c_str());
			}
			else
			{
				ImGui::Text(secondText.c_str());
			}
			break;

		case GUIType::SameLine:
			ImGui::SameLine();
			break;

		case GUIType::HelpMarker:
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(text.c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			break;

		case GUIType::Guizmo:
			if (scriptRef != 0)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				Matrix transform = scriptRef->transform.GetTransform();

				ImGuiIO& io = ImGui::GetIO();
				ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
				ImGuizmo::Manipulate(
					glm::value_ptr(Graphics::MainCamera()->GetViewProjection().coreMatrix),
					glm::value_ptr(Graphics::MainCamera()->_projectionMatrix.coreMatrix),
					ImGuizmo::TRANSLATE,
					ImGuizmo::LOCAL,
					glm::value_ptr(transform.coreMatrix)
				);
				break;
			}
		}

		if (font != nullptr)
		{
			ImGui::PopFont();
		}

		style.Colors[ImGuiCol_Text] = styleCol;
		style.Colors[ImGuiCol_WindowBg] = windowBgCol;

		style.WindowPadding = padding;
		style.WindowBorderSize = winBorderSize;
		style.ChildBorderSize = childWinBorderSize;

		style.WindowRounding = winBorderRadius;
		style.ChildRounding = childWinBorderRadius;
		style.FrameRounding = frameBorderRadius;

		for (int i = 0; i < onUIUpdateEndEvents.size(); i++)
		{
			onUIUpdateEndEvents[i]();
		}
	}
}
