#pragma once
#include <stdint.h>
#include <render/render.h>
#include <list>
#include <mutex>
#include <minwindef.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h""
//#include "imgui/imgui_impl_win32.h"

#pragma comment (lib, "d3d11.lib")
#include <D3D11.h>

#include <sdk/eft.h>
#include "doodle/draw.h" // doodle draws!

#include <menu/arial.ttf.h>



namespace config
{
	enum class aimbot_type
	{
		silent,
		memory
	};


	static const char* espbox_type_names[] = {
		"Corner Boxes",
		"Full Boxes"
	};

	enum class espbox_type
	{
		corner,
		full
	};


	inline bool show_fps = true;

	namespace aimbot
	{
		inline aimbot_type aimbot = aimbot_type::silent;

		inline bool enable_fov_circle = true;
		inline float fov = 250.0f;
		inline draw::color32 fov_color = draw::color32::from_rgba(255, 255, 255, 78);

		// segment increase factor
		// this will scale segments for performance
		// small circle, low segments
		// big circle, high(er)-ish segments
		inline const float fov_segments()
		#define CALC ((fov / 500.0f) * 150.0f)
		#define MIN 12.0f
		{
			return CALC < MIN ? MIN : CALC;
		}
	#undef CALC
	}

	namespace esp
	{
		inline bool enabled = true;

		namespace scav
		{

		}

		namespace pmc
		{

		}

		namespace player
		{
			// skeleton esp
			inline bool skeleton = true;

			inline draw::color32 skeleton_color = draw::color32::from_rgba(68, 101, 220, 160);
			inline int thickness = 2;


			// player distance
			inline bool distance = true;

			inline draw::color32 distance_color = draw::color32::from_rgba(201, 201, 201, 200);

			// player boxes
			inline bool boxes = true;

			inline espbox_type box_type = espbox_type::corner;
			inline draw::color32 boxes_color = draw::color32::from_rgba(184, 75, 75, 120);
		}
	}
}


namespace menu
{
	inline bool show = true;


	// menu stuff
	void load();

	void unload();
}