#pragma once

#include "ApiUtils.h"

#ifdef __cplusplus
extern "C"
{	
#endif
	GUI_TEST_API void HttpGetResult_Delete(void* ptr);
	GUI_TEST_API int HttpGetResult_GetResult(void* ptr);
	GUI_TEST_API size_t HttpGetResult_GetSize(void* ptr);
	GUI_TEST_API void* HttpGetResult_GetData(void* ptr);

	GUI_TEST_API void* HttpClient_New();
	GUI_TEST_API void HttpClient_Delete(void* ptr);	
	GUI_TEST_API void* HttpClient_Get(void* ptr, const char* url);

	typedef void (*HttpGetCallback)(void* ptr_result, void* userData);
	GUI_TEST_API void HttpClient_GetAsync(void* ptr, const char* url, HttpGetCallback callback, void* userData);

#ifdef __cplusplus
}
#endif