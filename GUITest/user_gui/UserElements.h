#pragma once

#include "ApiUtils.h"

#ifdef __cplusplus
extern "C"
{	
#endif
	GUI_TEST_API void Element_Delete(void* ptr);
	GUI_TEST_API const char* Element_GetName(void* ptr);
	GUI_TEST_API void Element_SetName(void* ptr, const char* name);
	GUI_TEST_API void* Text_New(const char* text);
	GUI_TEST_API void* SameLine_New();
	GUI_TEST_API void* InputText_New(const char* name, int size, const char* str);
	GUI_TEST_API const char* InputText_GetText(void* ptr);
	GUI_TEST_API void InputText_SetText(void* ptr, const char* text);
	GUI_TEST_API void* Button_New(const char* name);

	typedef void (*ClickCallback)(void* ptr);	
	GUI_TEST_API void Button_SetOnClick(void* ptr, ClickCallback callback, void* callback_data);

#ifdef __cplusplus
}
#endif
