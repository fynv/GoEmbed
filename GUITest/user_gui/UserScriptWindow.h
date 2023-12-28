#pragma once

#include "ApiUtils.h"

#ifdef __cplusplus
extern "C"
{
#endif
	GUI_TEST_API void* ScriptWindow_New();
	GUI_TEST_API void ScriptWindow_Delete(void* ptr);
	GUI_TEST_API int ScriptWindow_GetShow(void* ptr);
	GUI_TEST_API void ScriptWindow_SetShow(void* ptr, int show);
	GUI_TEST_API const char* ScriptWindow_GetTitle(void* ptr);
	GUI_TEST_API void ScriptWindow_SetTitle(void* ptr, const char* title);
	GUI_TEST_API void ScriptWindow_Add(void* ptr, void* ptr_elem);
	GUI_TEST_API void ScriptWindow_Remove(void* ptr, void* ptr_elem);
	GUI_TEST_API void ScriptWindow_Clear(void* ptr);

#ifdef __cplusplus
}
#endif

