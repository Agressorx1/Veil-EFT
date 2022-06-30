#include <Windows.h>
#include "eft.h"
#include <util/hook/memory_hook.h>



namespace eft
{
	// eft::player
	const bool player::is_local()
	{
		if (!self) return false;
		return functions::c_player::get_IsYourPlayer(this->self);
	}

	const bool player::is_AI()
	{
		if (!self) return false;
		return functions::c_player::get_IsAI(this->self);
	}


	player_physical player::physical()
	{
		if (!self) return NULL;
		return *(uintptr_t*)(this->self + functions::c_player::Physical);
	}

	player_body player::body()
	{
		if (!self) return NULL;
		return functions::c_player::get_PlayerBody(this->self);
	}



	// eft::player_body
	player_bones player_body::bones()
	{
		if (!self) return NULL;
		return *(uintptr_t*)(this->self + functions::c_playerbody::PlayerBones);
	}

	PlayerSide player_body::side()
	{
		if (!self) return PlayerSide::Unknown;
		return *(PlayerSide*)(this->self + functions::c_playerbody::_side);
	}

	player_skeleton player_body::root_skeleton()
	{
		if (!self) return NULL;
		return *(uintptr_t*)(this->self + functions::c_playerbody::SkeletonRootJoint);
	}


	// eft::player_skeleton
	csharp::clist<csharp::cstring> player_skeleton::bone_names()
	{
		if (!self) return NULL;
		return *(uintptr_t*)(this->self + functions::c_skeleton::_keys);
	}

	csharp::clist<unity::transform> player_skeleton::bone_transforms()
	{
		if (!self) return NULL;
		return *(uintptr_t*)(this->self + functions::c_skeleton::_values);
	}



	// eft::game_world
	csharp::clist<player> game_world::players()
	{
		if (!this->self) return NULL;
		return *(uintptr_t*)(this->self + functions::c_gameworld::RegisteredPlayers);
	}

	csharp::clist<uintptr_t> game_world::lootlist()
	{
		if (!this->self) return NULL;
		return *(uintptr_t*)(this->self + functions::c_gameworld::LootList);
	}
}



//
void cheat::update_loop()
{
	while (running)
	{
		cheat::game_world = get_game_world();

		cheat::camera = unity::camera::main_camera();
		
		
		if (cheat::game_world == NULL)
		{
			// reset
			cheat::local_player = NULL;
		}

		Sleep(50);
	}
}