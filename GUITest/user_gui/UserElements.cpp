#include "UserElements.h"
#include "Elements.h"

void Element_Delete(void* ptr)
{
	delete (Element*)ptr;
}

const char* Element_GetName(void* ptr)
{
	Element* elem = (Element*)ptr;
	return elem->name.c_str();
}

void Element_SetName(void* ptr, const char* name)
{
	Element* elem = (Element*)ptr;
	elem->name = name;
}

void* Text_New(const char* text)
{
	return new Text(text);
}

void* SameLine_New()
{
	return new SameLine;
}

void* InputText_New(const char* name, int size, const char* str)
{
	return new InputText(name, size, str);
}

const char* InputText_GetText(void* ptr)
{
	InputText* elem = (InputText*)ptr;
	return elem->text.data();
}

void InputText_SetText(void* ptr, const char* text)
{
	InputText* elem = (InputText*)ptr;
	size_t len = strlen(text);
	if (len > elem->text.size() - 1)
	{
		len = elem->text.size() - 1;
	}
	memcpy(elem->text.data(), text, len);
	elem->text[len] = 0;
}

void* Button_New(const char* name)
{
	return new Button(name);
}

void Button_SetOnClick(void* ptr, ClickCallback callback, void* callback_data)
{
	Button* elem = (Button*)ptr;
	elem->click_callback = callback;
	elem->click_callback_data = callback_data;
}

