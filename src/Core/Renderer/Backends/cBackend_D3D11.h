#pragma once

#include <Core/renderer/backends/iBackend.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

class cBackend_D3D11 : public iBackend
{
public:
	 cBackend_D3D11();
	~cBackend_D3D11();

	void create( cWindow& _window ) override;
	void clear( unsigned int _color ) override;
	
	void beginFrame( void ) override;
	void endFrame  ( void ) override;

private:

	int createSwapchainResources();
	void destroySwapchainResources();

	cWindow* m_window;

	Microsoft::WRL::ComPtr<IDXGIFactory2>          m_dxgi_factory   = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device>           m_device         = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_device_context = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>        m_swap_chain     = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target  = nullptr;

};
