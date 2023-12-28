#pragma once

#if defined(GUI_TEST_EXPORT) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#define GUI_TEST_API __declspec(dllexport)
#else
#define GUI_TEST_API 
#endif

