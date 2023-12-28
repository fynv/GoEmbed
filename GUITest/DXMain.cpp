#include <chrono>
#include <unordered_map>
#include <queue>
#include <windowsx.h>
#include "DXContext.h"
#include "DXMain.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

inline uint64_t time_micro_sec()
{
	std::chrono::time_point<std::chrono::system_clock> tpSys = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> tpMicro
		= std::chrono::time_point_cast<std::chrono::microseconds>(tpSys);
	return tpMicro.time_since_epoch().count();
}

inline uint64_t time_milli_sec()
{
	return (time_micro_sec() + 500) / 1000;
}

struct Action
{
	void(*act)(void*);
	void* userData;
};

inline void _SetWinDpiAwareness()
{
	HINSTANCE hinstLib = LoadLibraryA("user32.dll");

	BOOL(WINAPI * pSetProcessDpiAwarenessContext)(HANDLE);	
	pSetProcessDpiAwarenessContext = (decltype(pSetProcessDpiAwarenessContext))GetProcAddress(hinstLib, "SetProcessDpiAwarenessContext");
	if (pSetProcessDpiAwarenessContext != nullptr)
	{
		pSetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}
	else
	{
#ifndef DPI_ENUMS_DECLARED
		typedef enum
		{
			PROCESS_DPI_UNAWARE = 0,
			PROCESS_SYSTEM_DPI_AWARE = 1,
			PROCESS_PER_MONITOR_DPI_AWARE = 2
		} PROCESS_DPI_AWARENESS;
#endif

		HRESULT(WINAPI * pSetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
		pSetProcessDpiAwareness = (decltype(pSetProcessDpiAwareness))GetProcAddress(hinstLib, "SetProcessDpiAwareness");
		if (pSetProcessDpiAwareness != nullptr)
		{
			pSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
		}
		else
		{
			BOOL(WINAPI * pSetProcessDPIAware)(void);
			pSetProcessDPIAware = (decltype(pSetProcessDPIAware))GetProcAddress(hinstLib, "SetProcessDPIAware");
			if (pSetProcessDPIAware != nullptr)
			{
				pSetProcessDPIAware();
			}
		}
	}	

}

DXMain::DXMain(const wchar_t* title, int width, int height)
{
	_SetWinDpiAwareness();

	// Window
	{
		RECT rect{ 0,0,width, height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		m_hInstance = GetModuleHandle(NULL);
		WNDCLASS wc{};
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = m_hInstance;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = L"DXMain";
		RegisterClass(&wc);

		m_hWnd = CreateWindowEx(0, L"DXMain", title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, m_hInstance, nullptr);
		from_hwnd(m_hWnd) = this;

		ShowWindow(m_hWnd, SW_NORMAL);
	}

	// DX
	m_ctx = std::unique_ptr<DXContext>(new DXContext(m_hWnd));
	m_ctx->MakeCurrent();
	// timer
	m_timer = CreateThreadpoolTimer(s_TimerCallback, this, nullptr);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(m_ctx->m_pd3dDevice, m_ctx->m_pContext);
}

DXMain::~DXMain()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Timer
	SetThreadpoolTimer(m_timer, NULL, 0, 0);
	WaitForThreadpoolTimerCallbacks(m_timer, TRUE);
	CloseThreadpoolTimer(m_timer);

	// DX
	if (DXContext::s_pCurrent == m_ctx.get()) DXContext::s_pCurrent = nullptr;
	m_ctx = nullptr;

	// Window
	DestroyWindow(m_hWnd);
}


void DXMain::GetSize(int& width, int& height)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}


DXContext* DXMain::get_context()
{
	return m_ctx.get();
}

void DXMain::MainLoop()
{
	MSG msg;
	while (!m_abort && GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		idle();
	}
}

void DXMain::SetFramerate(float fps)
{
	m_interval = (unsigned)(1000.0f / fps);
	m_time_scheduled = time_milli_sec();
	RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
}

void DXMain::PostAction(void(*act)(void*), void* userData)
{
	Action* p_action = new Action{ act, userData };
	PostMessage(m_hWnd, WM_USER, 0, (LPARAM)p_action);
}


void DXMain::SetMouseCapture()
{
	SetCapture(m_hWnd);
}

void DXMain::ReleaseMouseCapture()
{
	ReleaseCapture();
}

DXMain*& DXMain::from_hwnd(HWND hwnd)
{
	static std::unordered_map<HWND, DXMain*> s_map;
	return s_map[hwnd];
}

LRESULT DXMain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;


	DXMain* self = from_hwnd(hWnd);
	switch (message)
	{
	case WM_USER:
	{
		Action* p_action = (Action*)lParam;
		p_action->act(p_action->userData);
		delete p_action;
		return 0;
	}
	case WM_SIZE:
	{
		if (self->m_ctx != nullptr)
		{
			self->m_ctx->RecreateSwapchain();
		}
		return 0;
	}
	case WM_PAINT:
	{
		if (self->m_ctx != nullptr)
		{		
			RECT rect;
			GetClientRect(hWnd, &rect);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			self->paint(width, height);
			self->m_ctx->Present();
		}

		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		if (self->m_interval > 0)
		{
			unsigned interval = self->m_interval;
			uint64_t t = time_milli_sec();
			int delta = (int)(int64_t)(t - self->m_time_scheduled);
			if (delta > interval) interval = 0;
			else if (delta > 0) interval -= delta;
			self->m_time_scheduled = t + interval;

			if (interval > 0)
			{
				ULARGE_INTEGER due;
				due.QuadPart = (ULONGLONG)(-((int64_t)interval * 10000LL));
				FILETIME ft;
				ft.dwHighDateTime = due.HighPart;
				ft.dwLowDateTime = due.LowPart;
				SetThreadpoolTimer(self->m_timer, &ft, 0, 0);
			}
			else
			{
				RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
			}
		}

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		self->mouseDown(0, 1, 0, x, y);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		self->mouseDown(1, 1, 0, x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		self->mouseDown(2, 1, 0, x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		self->mouseUp(0, 1, 0, x, y);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		self->mouseUp(1, 1, 0, x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		self->mouseUp(2, 1, 0, x, y);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		int button = -1;
		if (wParam == MK_LBUTTON) button = 0;
		if (wParam == MK_MBUTTON) button = 1;
		if (wParam == MK_RBUTTON) button = 2;
		self->mouseMove(button, 0, 0, x, y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		self->mouseWheel(-1, 0, zDelta, x, y);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void DXMain::s_TimerCallback(PTP_CALLBACK_INSTANCE, PVOID context, PTP_TIMER timer)
{
	DXMain* self = (DXMain*)context;
	RedrawWindow(self->m_hWnd, nullptr, nullptr, RDW_INVALIDATE);
}

enum class MessageType
{
	Paint = 0,
	Action = 1
};

struct Message
{
	MessageType type;
	void *param;
};

class DXMainWindowless::MessageQueue
{
public:
	MessageQueue()
	{
		InitializeCriticalSectionAndSpinCount(&m_cs, 0x00000400);
		m_hSemaphore = CreateSemaphore(NULL, 0, ~(1 << 31), NULL);
	}

	~MessageQueue()
	{
		CloseHandle(m_hSemaphore);
		DeleteCriticalSection(&m_cs);
	}

	size_t Size()
	{
		return m_queue.size();
	}

	void PushMessage(Message msg)
	{
		EnterCriticalSection(&m_cs);
		m_queue.push(msg);
		LeaveCriticalSection(&m_cs);
		ReleaseSemaphore(m_hSemaphore, 1, NULL);
	}

	Message PopMessage()
	{
		WaitForSingleObject(m_hSemaphore, INFINITE);
		EnterCriticalSection(&m_cs);
		Message msg = m_queue.front();
		m_queue.pop();
		LeaveCriticalSection(&m_cs);
		return msg;
	}

private:
	std::queue<Message> m_queue;
	CRITICAL_SECTION m_cs;
	HANDLE m_hSemaphore;
};

DXMainWindowless::DXMainWindowless()
{
	m_msg_queue = std::unique_ptr<MessageQueue>(new MessageQueue);
	// DX
	m_ctx = std::unique_ptr<DXContext>(new DXContext());
	m_ctx->MakeCurrent();
	// timer
	m_timer = CreateThreadpoolTimer(s_TimerCallback, this, nullptr);
}

DXMainWindowless::~DXMainWindowless()
{
	// Timer
	SetThreadpoolTimer(m_timer, NULL, 0, 0);
	WaitForThreadpoolTimerCallbacks(m_timer, TRUE);
	CloseThreadpoolTimer(m_timer);

	// DX
	if (DXContext::s_pCurrent == m_ctx.get()) DXContext::s_pCurrent = nullptr;
	m_ctx = nullptr;
}

DXContext* DXMainWindowless::get_context()
{
	return m_ctx.get();
}

void DXMainWindowless::MainLoop()
{
	while (!m_abort)
	{
		Message msg = m_msg_queue->PopMessage();
		if (msg.type == MessageType::Paint)
		{
			paint();
			if (m_interval > 0)
			{
				unsigned interval = m_interval;
				uint64_t t = time_milli_sec();
				int delta = (int)(int64_t)(t - m_time_scheduled);
				if (delta > interval) interval = 0;
				else if (delta > 0) interval -= delta;
				m_time_scheduled = t + interval;

				if (interval > 0)
				{
					ULARGE_INTEGER due;
					due.QuadPart = (ULONGLONG)(-((int64_t)interval * 10000LL));
					FILETIME ft;
					ft.dwHighDateTime = due.HighPart;
					ft.dwLowDateTime = due.LowPart;
					SetThreadpoolTimer(m_timer, &ft, 0, 0);
				}
				else
				{
					m_msg_queue->PushMessage({ MessageType::Paint, nullptr });
				}
			}
		}
		else if (msg.type == MessageType::Action)
		{
			Action* p_action = (Action*)msg.param;
			p_action->act(p_action->userData);
			delete p_action;
		}
		idle();
	}

}

void DXMainWindowless::SetFramerate(float fps)
{
	m_interval = (unsigned)(1000.0f / fps);
	m_time_scheduled = time_milli_sec();
	m_msg_queue->PushMessage({ MessageType::Paint, nullptr });
}

void DXMainWindowless::PostAction(void(*act)(void*), void* userData)
{
	Action* p_action = new Action{ act, userData };
	m_msg_queue->PushMessage({ MessageType::Action, p_action });
}

void DXMainWindowless::s_TimerCallback(PTP_CALLBACK_INSTANCE, PVOID context, PTP_TIMER timer)
{
	DXMainWindowless* self = (DXMainWindowless*)context;
	self-> m_msg_queue->PushMessage({ MessageType::Paint, nullptr });
}
