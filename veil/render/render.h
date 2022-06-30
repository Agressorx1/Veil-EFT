#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <time.h>


// util
#include <util/xor.hpp>
#include <util/hook/memory_hook.h>
#include "image.h"



#define WINDOW_CLASS_NAME "UnityWndClass"

namespace render
{
	inline memory_hook::memhook_context* present_hook = NULL;

	// render window viewport data
	class render_window
	{
	public:
		int width, height;
		HWND hwnd;

		render_window()
		{
			width = 0;
			height = 0;

			hwnd = NULL;
		}

		render_window(HWND hwnd, int width, int height)
		{
			this->width = width;
			this->height = height;

			this->hwnd = hwnd;
		}


		void set_size(int w, int h)
		{
			this->width = w;
			this->height = h;
		}

		bool operator!=(const render_window& size) { return this->width != size.width || this->height != size.height; }
		bool operator==(const render_window& size) { return this->width == size.width && this->height == size.height; }
	};

	inline render_window window = render_window();


	// open devices from rendering
	inline ID3D11Device* device = NULL;
	inline ID3D11DeviceContext* context = NULL;
	inline IDXGISwapChain* swap_chain = NULL;
	inline ID3D11RenderTargetView* render_target = NULL;


	// load/unloading of rendering system
	bool load();
	void unload();


	// resetting for gui interaction
	inline bool gui_render_loaded = false;

	static void reset_render()
	{
		gui_render_loaded = false;
	}


	// render events
	typedef void(*render_event_t)(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* context);

	inline render_event_t on_load = (render_event_t)NULL;
	inline render_event_t on_render = (render_event_t)NULL;
	inline render_event_t on_unload = (render_event_t)NULL;


	// render window events
	typedef void(*window_event_t)(const render_window& window);

	inline window_event_t on_resize = (window_event_t)NULL;

	// input events
	typedef LRESULT(CALLBACK* wndproc_event_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	inline WNDPROC hooked_wndproc = NULL; // for event V
	inline wndproc_event_t on_wndproc = (wndproc_event_t)NULL;
}

namespace fps
{
	static double clock_to_milliseconds(clock_t ticks)
	{
		// units/(units/time) = time (seconds) * 1000 = milliseconds
		return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
	}

	inline clock_t previous_frame, delta_time;
	inline uint32_t frames = 0;
	inline double frame_rate = 30.0;
	inline double frame_time = 33.3333;
}