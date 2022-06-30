#include "menu.h"
#include <loader/main.h>
#include "doodle/draw.h"



void on_load(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* context);
void on_unload(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* context);
void on_render(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* context);

LRESULT WINAPI wndproc_handler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

void menu::load()
{
	render::on_load = on_load;
	render::on_render = on_render;
	//render::on_unload = on_unload;

	render::on_wndproc = wndproc_handler;
}



void menu::unload()
{
	render::on_load = NULL;
	render::on_render = NULL;
	render::on_unload = NULL;

	render::on_wndproc = NULL;

	on_unload(NULL, NULL, NULL);
}





// input
IMGUI_API LRESULT   ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );


LRESULT WINAPI wndproc_handler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
		return true;

	ImGuiContext* ctx = nullptr;
	if (menu::show)
	{
		if ((ctx = ImGui::GetCurrentContext()) != NULL)
			ctx->IO.MouseDrawCursor = true;
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	if ((ctx = ImGui::GetCurrentContext()) != NULL)
		ctx->IO.MouseDrawCursor = false;
	return CallWindowProc(render::hooked_wndproc, hWnd, Msg, wParam, lParam);
}




// loading
void on_load(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* context)
{
	// create imgui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// set imgui styles
	ImGui::StyleColorsDark();

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowPadding = ImVec2(10, 10);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 15.0f;
	style->GrabRounding = 3.0f;


	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);


	ImFontConfig* font_config = 0;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x044F, // Cyrillic
		0,
	};

	DEBUG("[menu]: loading font...\n");
	ImFont* font = io.Fonts->AddFontFromMemoryTTF(arial_ttf, sizeof(arial_ttf), 13.0f, NULL, ranges);
	if (!font->IsLoaded()) io.Fonts->Build();
	DEBUG("[menu]: loaded font.\n");

	//if (font && font->IsLoaded())
	//{
	//	ImGui::PushFont(font);
	//	DEBUG("[menu]: set font successfully.\n");
	//}
	//else
	//	DEBUG("[menu]: failed to set font.\n");

	HWND window = FindWindow( c( "UnityWndClass" ), NULL );
	// initialize imgui
	ImGui_ImplDX11_Init( window , device, context );
	ImGui_ImplDX11_CreateDeviceObjects( );
	DEBUG( "[menu]: loading font...\n" );

	render::gui_render_loaded = true;
}

void on_unload(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* context)
{
	ImGui_ImplDX11_Shutdown();

	render::reset_render();
}



// menu renders
void aimbot_tab()
{
	ImGui::Checkbox(c("FOV Circle"), &config::aimbot::enable_fov_circle);
	if (config::aimbot::enable_fov_circle)
	{
		ImGui::Text(c("Field of View"));
		ImGui::SliderFloat(c("##fov_circle"), &config::aimbot::fov, 1, 500, c("%.3g"));

		ImGui::Text(c("FOV Circle Color"));
		ImGui::SameLine();
		ImGui::ColorEdit4(c("##fov_circle_col"), (float*)&config::aimbot::fov_color.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);

		ImGui::Separator();
	}
}


void esp_tab()
{
	ImGui::Checkbox(c("ESP"), &config::esp::enabled);
	
	ImGui::Text(c("Player ESP")); ImGui::SameLine();
	ImGui::Separator();

	ImGui::Checkbox(c("Skeleton"), &config::esp::player::skeleton);
	if (config::esp::player::skeleton)
	{
		ImGui::Text(c("Bone Thickness"));
		ImGui::SliderInt(c("##fov_circle"), &config::esp::player::thickness, 1, 5);

		ImGui::Text(c("Bone Color"));
		ImGui::SameLine();
		ImGui::ColorEdit4(c("##skeleton_bone_col"), (float*)&config::esp::player::skeleton_color.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
	}

	ImGui::Separator();

	ImGui::Checkbox(c("Boxes"), &config::esp::player::boxes);
	if (config::esp::player::boxes)
	{
		ImGui::Combo(c("Box Type"), (int*)&config::esp::player::box_type, config::espbox_type_names, IM_ARRAYSIZE(config::espbox_type_names));

		ImGui::Text(c("Box Color"));
		ImGui::SameLine();
		ImGui::ColorEdit4(c("##esp_box_col"), (float*)&config::esp::player::boxes_color.R, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
	}

	ImGui::Separator();
}


void hacks_tab()
{
}



void draw_skeleton(eft::player player, draw::color32 color = draw::color32(), int thickness = 1)
{
	std::list<eft::PlayerBoneId> upper_part = {
		eft::PlayerBoneId::HumanNeck,
		eft::PlayerBoneId::HumanHead
	};
	std::list<eft::PlayerBoneId> right_arm = {
		eft::PlayerBoneId::HumanNeck,
		eft::PlayerBoneId::HumanRUpperarm,
		eft::PlayerBoneId::HumanRForearm1,
		eft::PlayerBoneId::HumanRPalm
	};
	std::list<eft::PlayerBoneId> left_arm = {
		eft::PlayerBoneId::HumanNeck,
		eft::PlayerBoneId::HumanLUpperarm,
		eft::PlayerBoneId::HumanLForearm1,
		eft::PlayerBoneId::HumanLPalm
	};
	std::list<eft::PlayerBoneId> spine = {
		eft::PlayerBoneId::HumanNeck,
		eft::PlayerBoneId::HumanSpine1,
		eft::PlayerBoneId::HumanSpine2,
		eft::PlayerBoneId::HumanPelvis
	};

	std::list<eft::PlayerBoneId> lower_right = {
		eft::PlayerBoneId::HumanPelvis,
		eft::PlayerBoneId::HumanRThigh1,
		eft::PlayerBoneId::HumanRCalf,
		eft::PlayerBoneId::HumanRFoot
	};
	std::list<eft::PlayerBoneId> lower_left = {
		eft::PlayerBoneId::HumanPelvis,
		eft::PlayerBoneId::HumanLThigh1,
		eft::PlayerBoneId::HumanLCalf,
		eft::PlayerBoneId::HumanLFoot
	};
	std::list<std::list<eft::PlayerBoneId>> skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };


	vector3f previous(0, 0, 0);
	vector3f current;
	vector3f p1 = vector3f();
	vector3f c1 = vector3f();
	vector3f neckpos = player.bone(eft::PlayerBoneId::HumanNeck).position();
	vector3f pelvispos = player.bone(eft::PlayerBoneId::HumanPelvis).position();

	for (std::list<eft::PlayerBoneId> bone_list : skeleton)
	{
		previous = vector3f();
		for (eft::PlayerBoneId bone : bone_list)
		{
			current = bone == eft::PlayerBoneId::HumanNeck ? neckpos : (bone == eft::PlayerBoneId::HumanPelvis ? pelvispos : player.bone(bone).position());
			if (previous.x == 0.f)
			{
				previous = current;
				continue;
			}

			if (cheat::camera.world_to_screen(current, c1))
			{
				if (cheat::camera.world_to_screen(previous, p1))
				{
					draw::line(p1.x, p1.y, c1.x, c1.y, color, thickness);
					previous = current;
				}
			}
		}

	}
}


void draw_distance(vector3f position, vector3f screen_pos, draw::color32 color)
{
	float distance = position.distance(cheat::local_player.position());

	char dist_str[40];
	memset(dist_str, 0, sizeof(dist_str));
	if (sprintf_s<sizeof(dist_str)>(dist_str, "%.1fm", distance) == -1)
		return;

	ImVec2 tsize = ImGui::CalcTextSize(dist_str);
	draw::text(screen_pos.x, screen_pos.y, color, dist_str);
}


void draw_box(vector3f head_screen_pos, vector3f screen_pos, draw::color32 color, config::espbox_type box_type)
{
	float height = head_screen_pos.y - screen_pos.y;
	float width = (height / 2) * 1.2f;

	switch (box_type)
	{
		case config::espbox_type::corner:
		{
			draw::corner_box(screen_pos.x, screen_pos.y, width, height, 2, color);
			break;
		}
		case config::espbox_type::full:
		{
			draw::box(screen_pos.x, screen_pos.y, width, height, color);
		}
	}
}

// cheat
uintptr_t get_game_world( )
{
	unity::game_object object = unity::game_object::find_object( csharp::cstring( "GameWorld" ) );
	if ( !object ) return NULL;

	// idk how this works :(
	uintptr_t cptr = *( uintptr_t * ) ( object + 0x10 );
	if ( !cptr ) return NULL;

	uintptr_t smthn = *( uintptr_t * ) ( cptr + 0x30 );
	if ( !smthn ) return NULL;

	uintptr_t smthn2 = *( uintptr_t * ) ( smthn + 0x18 );
	if ( !smthn2 ) return NULL;

	return *( uintptr_t * ) ( smthn2 + 0x28 );
}

ImGuiWindow &createscene( )
{
	ImGui_ImplDX11_NewFrame( );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0 );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
	ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0, 0, 0, 0 ) );
	ImGui::Begin( xorstr( "##createscene" ), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar );
	auto &io = ImGui::GetIO( );
	ImGui::SetWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
	ImGui::SetWindowSize( ImVec2( io.DisplaySize.x, io.DisplaySize.y ), ImGuiCond_Always );
	return *ImGui::GetCurrentWindow( );
}

VOID destroyscene( ImGuiWindow &window )
{

	window.DrawList->PushClipRectFullScreen( );
	ImGui::End( );
	ImGui::PopStyleColor( );
	ImGui::PopStyleVar( 2 );
	ImGui::Render( );
}
// menu
void on_render(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* context)
{

	//cheat::game_world = get_game_world( );

	//cheat::camera = unity::camera::main_camera( );


	//if ( cheat::game_world == NULL )
	//{
	//	// reset
	//	cheat::local_player = NULL;
	//}

	//// begin imgui render
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
	//auto &window = createscene( );

	if (config::veil_watermark)
	{
		ImGui::GetOverlayDrawList( )->AddRectFilled( ImVec2( 15, 15 ), ImVec2( 265, 52 ), ImColor( 0, 0, 0, 150 ) );
		ImGui::GetOverlayDrawList( )->AddRect( ImVec2( 15, 15 ), ImVec2( 265, 52 ), ImColor( 173, 195, 230, 255 ) );
		ImGui::GetOverlayDrawList( )->AddText( ImVec2( 22, 24 ), ImColor( 255, 255, 255 ), ( c( "  veil eft-engine v2" ) ) );
	}


	if (config::aimbot::enable_fov_circle)
		draw::circle(render::window.width / 2, render::window.height / 2,
			config::aimbot::fov, config::aimbot::fov_color,
			config::aimbot::fov_segments());


	if (GetAsyncKeyState(KEYBIND_TOGGLE_MENU) & 1)
		menu::show = !menu::show;



	if (cheat::camera)
	{
		csharp::clist<eft::player> players = cheat::game_world.players();
		for (uint32_t i = 0; i < players.size(); i++)
		{
			eft::player plr = players[i];
			if (plr.is_local())
			{
				cheat::local_player = plr;
				continue;
			}

			eft::player_skeleton skel = plr.body().root_skeleton();
			if (!skel)
				continue;

			if (config::esp::enabled)
			{
				vector3f position = plr.position();
				vector3f head_pos = plr.bone(eft::PlayerBoneId::HumanHead).position();
				
				vector3f screen_pos{};
				if (!cheat::camera.world_to_screen(position, screen_pos))
					continue;

				vector3f head_screen_pos{};
				if (!cheat::camera.world_to_screen(head_pos, head_screen_pos))
					continue;

				if (config::esp::player::skeleton)
				{
					draw_skeleton(
						plr,
						config::esp::player::skeleton_color,
						config::esp::player::thickness
					);
				}

				if (config::esp::player::distance)
				{
					draw_distance(
						position,
						screen_pos,
						config::esp::player::distance_color
					);
				}

				if (config::esp::player::boxes)
				{
					draw_box(
						head_screen_pos,
						screen_pos,
						config::esp::player::boxes_color,
						config::esp::player::box_type
					);
				}
			}

			//csharp::clist<unity::transform> bones = skel.bone_transforms();
			//draw_text(bones[(int32_t)eft::PlayerBoneId::BaseHuman].position(), "BaseHuman");
		}
	}



	if (menu::show)
	{
		ImGui::SetNextWindowSize(ImVec2(390, 470));
		ImGui::Begin(c("Veil"), NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize);


		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(c("Veil"))) {
				//if (ImGui::MenuItem(c("Theme Editor")))
				//{
				//	upcoming();
				//}

				if (ImGui::MenuItem(c("Toggle FPS"))) config::show_fps = !config::show_fps;
				if (ImGui::MenuItem(c("Fuck This Game"))) TerminateProcess(GetCurrentProcess(), 0);

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}


		ImGui::End();
	}

	if (config::show_fps)
		ImGui::GetOverlayDrawList()->AddText(ImVec2(5, 5), ImColor(0.0f, 0.5f, 0.0f, 1.0f), std::to_string((int)fps::frame_rate).c_str());

	// render imgui
	ImGui::Render( );
	//destroyscene( window ); //Destroy
}