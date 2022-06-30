#include "render.h"
#include <Windows.h>

#include <loader/main.h>



bool present_loaded = false;
bool render_loaded = false;

HWND target_hwnd, render_hwnd;

bool resized = false;
//---------------------------------------



// resizing handler
LRESULT CALLBACK hWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				int width = (UINT)LOWORD(lParam), height = (UINT)HIWORD(lParam);
				if (render::render_target) { render::render_target->Release(); render::render_target = nullptr; }
				render::swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
				render::window = render::render_window(hWnd, width, height);
				resized = true;

				if (render::on_resize) render::on_resize(render::window);
				return CallWindowProc(render::hooked_wndproc, hWnd, Msg, wParam, lParam);
			}
			break;
		}
	}

	if (render::on_wndproc)
		return render::on_wndproc(hWnd, Msg, wParam, lParam);
	return CallWindowProc(render::hooked_wndproc, hWnd, Msg, wParam, lParam);
}



HRESULT(__stdcall *o_Present)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags) = 0;
HRESULT __stdcall Present_H(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!present_loaded)
	{
		DEBUG("[render]: first present call, initializing...\n");
		
		render::swap_chain = pSwapChain;
		if (FAILED(render::swap_chain->GetDevice(__uuidof(render::device), (void**)&render::device)))
		{
			DEBUG("[render]: failed to fallback to secondary SwapChain grabation.\n");
			return 0;
		}

		render::device->GetImmediateContext(&render::context);

		ID3D11Texture2D *back_buffer;
		if (FAILED(render::swap_chain->GetBuffer(0, __uuidof(back_buffer), (void **)&back_buffer)))
		{
			DEBUG("[render]: failed GetBuffer in Present_H.\n");
			return 0;
		}

		if (FAILED(render::device->CreateRenderTargetView((ID3D11Resource *)back_buffer, NULL, &render::render_target)))
		{
			DEBUG("[render]: Failed CreaterenderTargetView in Present_H.\n");
			return 0;
		}
		back_buffer->Release();

		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;
		render::context->RSGetViewports(&num_viewports, &viewport);
		
		render::window.set_size((int32_t)viewport.Width, (int32_t)viewport.Height);
		
		if (render::on_load) render::on_load(pSwapChain, render::device, render::context);
		DEBUG("[render]: initialized rendering successfully.\n");
		present_loaded = true;
	}

	if (!render::gui_render_loaded && render::on_load)
		render::on_load(render::swap_chain, render::device, render::context);

	if (resized)
	{
		ID3D11Texture2D *back_buffer;
		if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&back_buffer)))
		{
			if (FAILED(render::device->CreateRenderTargetView(back_buffer, NULL, &render::render_target)))
			{
				DEBUG("[render]: RenderTargetView was unsuccessful reinitializing for resizing.\n");
				back_buffer->Release();
				return 0;
			}
			back_buffer->Release();
			DEBUG("[render]: RenderTargetView reinitialized successfully for resizing.\n");
			resized = false;
		}
	}

	render::context->OMSetRenderTargets(1, &render::render_target, NULL);


	if (render::on_render) render::on_render(render::swap_chain, render::device, render::context);


	// calculate FPS
	if (fps::frames == 0)
	{
		fps::previous_frame = clock();
		fps::frames++;
	}
	else
	{
		clock_t frame = clock();

		fps::delta_time += frame - fps::previous_frame;
		fps::frames++;

		if (fps::clock_to_milliseconds(fps::delta_time) > 1000.0)
		{
			fps::frame_rate = (double)fps::frames * 0.5 + fps::frame_rate * 0.5;
			fps::frames = 0;
			fps::delta_time -= CLOCKS_PER_SEC;
			fps::frame_time = 1000.0 / (fps::frame_rate == 0 ? 0.001 : fps::frame_rate);
		}

		fps::previous_frame = frame;
	}
	return o_Present(pSwapChain, SyncInterval, Flags);
}




/* render loading/unloading */
LRESULT CALLBACK DefaultWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

bool render::load()
{
	if (render_loaded)
		return false;

	target_hwnd = FindWindow(c(WINDOW_CLASS_NAME), NULL);
	if (!target_hwnd)
	{
		DEBUG("[render]: unable to find Tarkov window.\n");
		return false;
	}
	render::window.hwnd = target_hwnd;

	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL obtainedLevel;
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = target_hwnd;
	scd.SampleDesc.Count = 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;

	scd.BufferDesc.RefreshRate.Numerator = 1;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	
	DEBUG("[render]: creating swapchain...\n");
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &scd, &render::swap_chain, &render::device, &obtainedLevel, &render::context)))
	{
		DEBUG("[render]: unable to create temporary device for swap chain vtable.\n");
		return false;
	}
	DEBUG("[render]: created proxy swap chain and device successfully.\n");

	/* get the vtable and hook accordingly */
	uintptr_t *swap_chain_vtable = *(uintptr_t **)render::swap_chain;

	//swap_chain_vtable[8] // present
	//context_vtable[12] // drawindexed
	//device_vtable[24] // createquery
	//context_vtable[8] // pssetshaderresources
	//swap_chain_vtable[50] // clearrendertargetview

	DEBUG("[render]: hooking...\n");
	present_hook = memory_hook::create_context(swap_chain_vtable[8]);
	o_Present = present_hook->hook(Present_H);
	present_hook->enable_hook();

	render::hooked_wndproc = (WNDPROC)SetWindowLongPtr(target_hwnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);
	DEBUG("[render]: hooked.\n");

	render_loaded = true;
	return true;
}

void render::unload()
{
	if (!render_loaded)
		return;

	SetWindowLongPtr(target_hwnd, GWLP_WNDPROC, (LONG_PTR)render::hooked_wndproc);

	memory_hook::destroy_context(present_hook);


	if (render::on_unload) render::on_unload(render::swap_chain, render::device, render::context);
	render::gui_render_loaded = false;

	if (render::swap_chain)
		render::swap_chain->Release();
	if (render::context)
		render::context->Release();
	if (render::device)
		render::device->Release();
	if (render::render_target)
		render::render_target->Release();

	render_loaded = false;
	present_loaded = false;
}