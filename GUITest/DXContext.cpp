#include "DXContext.h"

#pragma comment(lib, "D3D11.lib")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif

thread_local DXContext* DXContext::s_pCurrent = nullptr;

DXContext::DXContext(HWND hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	static const UINT createDeviceFlags = 0;
	static const D3D_FEATURE_LEVEL flv = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL selected_flv;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &flv, 1,
		D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &selected_flv, &m_pContext);

	ID3D11Texture2D* pBackBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	pBackBuffer->Release();
}

DXContext::DXContext()
{
	static const UINT createDeviceFlags = 0;
	static const D3D_FEATURE_LEVEL flv = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL selected_flv;
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &flv, 1, D3D11_SDK_VERSION, &m_pd3dDevice, &selected_flv, &m_pContext);
}

DXContext::~DXContext()
{
	if (m_pContext) m_pContext->ClearState();
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pd3dDevice);
}

void DXContext::MakeCurrent()
{
	if (m_pRenderTargetView != nullptr)
	{
		m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
	}
	s_pCurrent = this;
}

void DXContext::Present(bool vsync)
{
	if (m_pSwapChain != nullptr)
	{
		m_pSwapChain->Present(vsync?1:0, 0);
	}
}

void DXContext::RecreateSwapchain()
{
	if (m_pSwapChain!=nullptr)
	{
		m_pContext->OMSetRenderTargets(0, 0, 0);
		m_pRenderTargetView->Release();
		m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D* pBackBuffer = nullptr;
		m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
		pBackBuffer->Release();

		m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
	}
}

