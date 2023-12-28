#pragma once

#include <cstdint>
#include <Windows.h>
#include <memory>

class DXContext;

class DXMainBase
{
public:
	virtual ~DXMainBase() {}
	virtual DXContext* get_context() = 0;
	virtual void MainLoop() = 0;
	void Abort()
	{
		m_abort = true;
	}
	virtual void SetFramerate(float fps) = 0;
	virtual void PostAction(void(*act)(void*), void* userData) = 0;

protected:
	DXMainBase() {}
	bool m_abort = false;
};

class DXMain : public DXMainBase
{
public:
	DXMain(const wchar_t* title, int width, int height);
	~DXMain();

	void GetSize(int& width, int& height);

	virtual DXContext* get_context() override;
	virtual void MainLoop() override;	
	virtual void SetFramerate(float fps) override;
	virtual void PostAction(void(*act)(void*), void* userData) override;

	void SetMouseCapture();
	void ReleaseMouseCapture();

protected:
	virtual void idle() {}
	virtual void paint(int width, int height) {}
	virtual void mouseDown(int button, int clicks, int delta, int x, int y) {}
	virtual void mouseUp(int button, int clicks, int delta, int x, int y) {}
	virtual void mouseMove(int button, int clicks, int delta, int x, int y) {}
	virtual void mouseWheel(int button, int clicks, int delta, int x, int y) {}

private:
	static DXMain*& from_hwnd(HWND hwnd);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HINSTANCE m_hInstance;
	HWND m_hWnd;

	std::unique_ptr<DXContext> m_ctx;

	unsigned m_interval = 0;
	uint64_t m_time_scheduled; 
	PTP_TIMER m_timer = nullptr;

	static void __stdcall  s_TimerCallback(PTP_CALLBACK_INSTANCE, PVOID context, PTP_TIMER timer);
};

class DXMainWindowless : public DXMainBase
{
public:
	DXMainWindowless();
	~DXMainWindowless();

	virtual DXContext* get_context() override;
	virtual void MainLoop() override;	
	virtual void SetFramerate(float fps) override;
	virtual void PostAction(void(*act)(void*), void* userData) override;

protected:
	virtual void idle() {}
	virtual void paint() {}

private:
	class MessageQueue;
	std::unique_ptr<MessageQueue> m_msg_queue;
	std::unique_ptr<DXContext> m_ctx;
	
	unsigned m_interval = 0;
	uint64_t m_time_scheduled;
	PTP_TIMER m_timer = nullptr;

	static void __stdcall  s_TimerCallback(PTP_CALLBACK_INSTANCE, PVOID context, PTP_TIMER timer);
};