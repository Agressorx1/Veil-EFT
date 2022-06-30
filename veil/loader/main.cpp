#include "main.h"
#include <render/render.h>
#include <Windows.h>
#include <thread>

#include <util/hook/memory_hook.h>

#include <sdk/eft.h>
#include <menu/menu.h>


// Debug Console
#ifdef PROJECT_DEBUG
void init_console()
{
	if (GetConsoleWindow())
	{
		system("cls");
		fclose(stdout);
		fclose(stdin);
	}
	else
		AllocConsole();

	freopen("conout$", "w", stdout);
	freopen("conin$", "r", stdin);
	
	Sleep(250);
	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	ShowWindow(GetConsoleWindow(), SW_NORMAL);
}
#endif

uintptr_t(*o_CreateShot)(uintptr_t self, uintptr_t ammo, vector3f origin, vector3f direction, int fireIndex,
	uintptr_t player, uintptr_t weapon, float speedFactor, int fragmentIndex);


eft::player get_target()
{
	float ofov = config::aimbot::fov;
	float nfov = 0;

	eft::player target = NULL;

	for (eft::player plr : cheat::game_world.players())
	{
		if (plr.is_local())
			continue;

		vector3f pos = plr.position();
		if (pos != vector3f())
		{
			vector3f from_center = cheat::camera.world_to_screen_from_point(pos, vector3f(render::window.width / 2.0f, render::window.height / 2.0f, 0));
			if (from_center.z < 0)
				continue;

			nfov = from_center.x + from_center.y;
			if (nfov == 0)
				continue;

			if (nfov < ofov)
			{
				ofov = nfov;
				if (target)
				{
					// last "aimbot target" distance from player
					float fLastTargetDist = cheat::local_player.position().distance(target.position());
					
					// new "aimbot target" distance from player
					float fTargetDist = cheat::local_player.position().distance(plr.position());

					// set aimbot target to new target if closer than last target
					if (fTargetDist < fLastTargetDist)
						target = plr;
				}
				else
					target = plr;
			}
		}
	}

	return target;
}


#include <d3dx10math.h>

uintptr_t CreateShot_H(uintptr_t self, uintptr_t ammo, vector3f origin, vector3f direction,
	int fireIndex, uintptr_t player, uintptr_t weapon, float speedFactor, int fragmentIndex)
{
	if (cheat::game_world && player == cheat::local_player)
	{
		eft::player target = get_target();
		vector3f head = vector3f();
		if (target && (head = target.bone(eft::PlayerBoneId::HumanHead).position()) != vector3f())
			direction = (head - origin).normalize();
	}
	return o_CreateShot(self, ammo, origin, direction, fireIndex, player, weapon, speedFactor, fragmentIndex);
}



static uintptr_t dll_start = 0, dll_end = 0;

LONG WINAPI exception_dumper(PEXCEPTION_POINTERS exception)
{
	uintptr_t addr = exception->ContextRecord->Rip;
	if (addr <= dll_end && addr >= dll_start)
	{
		DEBUG("Veil Error @ 0x%p\n", addr);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else
		DEBUG("Unknown Unhandled Exception @ 0x%p\n", addr);
	return EXCEPTION_EXECUTE_HANDLER;
}

int load_cheat()
{
#ifdef PROJECT_DEBUG
	//init_console();
#endif
	init_console( );
	//MessageBox(NULL, "", "", MB_OK);
	//uintptr_t hNtDll = util::get_process_module("ntdll.dll");
	//if (!hNtDll)
	//{
	//	DEBUG("[exception_handler]: unable to find ntdll.dll.\n");
	//	return 1;
	//}
	//uintptr_t set_handler = util::get_proc_address(hNtDll, "RtlSetUnhandledExceptionFilter");
	//if (!set_handler)
	//{
	//	DEBUG("[exception_handler]: unable to find RtlSetUnhandledExceptionFilter.\n");
	//	return 1;
	//}

	//((VOID(WINAPI*)(LPTOP_LEVEL_EXCEPTION_FILTER))set_handler)(exception_dumper);
	//DEBUG("[exception_handler]: initialized.\n");
	//Sleep(1000);


	functions::load_mono();

	//menu::load();
	//render::load();

	//std::thread update(cheat::update_loop);
	//update.detach();



	memory_hook::memhook_context* hctx = memory_hook::create_context(functions::c_ballistics_calculator::CreateShot);
	o_CreateShot = hctx->hook(CreateShot_H);
	hctx->enable_hook();


//#ifdef PROJECT_DEBUG
//	while (running)
//	{
//		if (GetAsyncKeyState(KEYBIND_UNLOAD) & 1)
//		{
//			if (GetForegroundWindow() == render::window.hwnd)
//				running = false;
//		}
//		Sleep(5);
//	}
//
//
//	DEBUG("[veil]: unloading...\n");
//	menu::unload();
//	//render::unload();
//
//	//if (a.joinable()) a.join();
//	if (update.joinable()) update.join();
//
//	//mono::mono::cleanup();
//	//memory_hook::destroy_all_contexts();
//
//	//FreeConsole();
//#endif
	return 0;
}


//-- Bootloader --//
struct dll_info
{
	uintptr_t dll_start;
	uintptr_t dll_end;
};


auto DllMain( void *, std::uint32_t call_reason, void * ) -> bool
{
	if ( call_reason != 1 )
		return false;

	if ( !load_cheat( ) )
		return false;

	return true;
}
