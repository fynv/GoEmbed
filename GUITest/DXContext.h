#pragma once

#include <d3d11_1.h>

class DXContext
{
public:
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	static thread_local DXContext* s_pCurrent;

	DXContext(HWND hWnd);
	DXContext();
	~DXContext();

	void MakeCurrent();
	void Present(bool vsync = false);
	void RecreateSwapchain();	

};

