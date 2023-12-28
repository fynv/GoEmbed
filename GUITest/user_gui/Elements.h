#pragma once

#include <string>
#include <vector>

class DXMain;
class Element
{
public:
	Element() {};
	virtual ~Element() {};
	std::string name;
	virtual void Draw(DXMain* main_wnd) = 0;
};

class Text : public Element
{
public:
	Text(const char* text);
	void Draw(DXMain* main_wnd) override;
};

class SameLine : public Element
{
public:
	void Draw(DXMain* main_wnd) override;
};

class InputText : public Element
{
public:
	std::vector<char> text;
	InputText(const char* name, int size = 256, const char* str = nullptr);

	void Draw(DXMain* main_wnd) override;

};

typedef void (*ClickCallback)(void* ptr);

class Button : public Element
{
public:
	ClickCallback click_callback = nullptr;
	void* click_callback_data = nullptr;

	Button(const char* name);
	void Draw(DXMain* main_wnd) override;
};


