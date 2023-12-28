#include "Elements.h"
#include "imgui.h"
#include "DXMain.h"

///////////////////// Text ///////////////////

Text::Text(const char* text)
{
	this->name = text;
}

void Text::Draw(DXMain* main_wnd)
{
	ImGui::Text(name.c_str());
}

///////////////////// SameLine ///////////////////
void SameLine::Draw(DXMain* main_wnd)
{
	ImGui::SameLine();
}

///////////////////// InputText ///////////////////

InputText::InputText(const char* name, int size, const char* str) : text(size)
{
	this->name = name;

	if (str != nullptr)
	{
		sprintf(text.data(), "%s", str);
	}
}

void InputText::Draw(DXMain* main_wnd)
{
	ImGui::InputText(name.c_str(), text.data(), text.size());
}

///////////////////// Button ///////////////////

Button::Button(const char* name)
{
	this->name = name;
}


void Button::Draw(DXMain* main_wnd)
{
	if (ImGui::Button(name.c_str()))
	{
		if (click_callback != nullptr)
		{
			main_wnd->PostAction(click_callback, click_callback_data);
		}
	}
}

