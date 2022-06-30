#pragma once
#include <stdint.h>
#include <mono/mono.h>
#include <array>

#include <sdk/unity/unity_structs.h>



// -------- UnityPlayer.dll Offsets -------
// UnityEngine.Camera
#define CAMERA_GET_MAIN 0x18092C250				// Camera_Get_Custom_PropMain
#define CAMERA_WORLD_TO_SCREEN 0x180929890			// Camera_CUSTOM_WorldToScreenPoint_Injected

// UnityEngine.Transform
#define TRANSFORM_GET_POSITION 0x1809960A0			// Transform_CUSTOM_get_position_Injected

// UnityEngine.Component
#define COMPONENT_GET_TRANSFORM 0x180936D20			// Component_Get_Custom_PropTransform
#define COMPONENT_GET_GAMEOBJECT 0x180936C20			// Component_Get_Custom_PropGameObject


// -----------------------------------

#define IDA_BASE 0x180000000
#define RVA_OFFSET(ida_addr) (ida_addr - IDA_BASE)



namespace unity
{
	enum MonoOrStereoscopicEye
	{
		Left,
		Right,
		Mono
	};
}

namespace functions
{
	inline bool mono_loaded = false;

	
	// ----- project/game ------
	namespace c_gameworld
	{
		inline uintptr_t gameworld_ctor = NULL;
		inline uintptr_t gameworld_dispose = NULL;

		inline uintptr_t(*get_AllPlayers)(uintptr_t self) = NULL;

		inline uint32_t RegisteredPlayers = NULL;
		inline uint32_t LootList = NULL;
	}

	namespace c_player
	{
		inline uintptr_t(__thiscall* get_IsYourPlayer)(uintptr_t self) = NULL;
		inline uintptr_t(__thiscall* get_IsAI)(uintptr_t self) = NULL;

		inline uintptr_t(__thiscall* get_PlayerBody)(uintptr_t self) = NULL;


		inline uint32_t Physical = NULL;
	}

	namespace c_playerbody
	{
		inline uint32_t PlayerBones = NULL;
		inline uint32_t _side = NULL;
		inline uint32_t SkeletonRootJoint = NULL;
	}

	namespace c_skeleton
	{
		inline uint32_t _keys = NULL;
		inline uint32_t _values = NULL;
	}


	namespace c_ballistics_calculator
	{
		inline uintptr_t(__thiscall* CreateShot)(uintptr_t self, uintptr_t ammo, vector3f origin, vector3f direction, int fireIndex, uintptr_t player, uintptr_t weapon, float speedFactor, int fragmentIndex);
	}


	// ----- unity engine ------
	namespace unity
	{
		namespace c_camera
		{
			inline uintptr_t(*get_main)() = (decltype(get_main))RVA_OFFSET(CAMERA_GET_MAIN);
			inline void(__fastcall *WorldToScreenPoint)(uintptr_t self, const vector3f* position, ::unity::MonoOrStereoscopicEye eye, vector3f* screen_pos) = (decltype(WorldToScreenPoint))RVA_OFFSET(CAMERA_WORLD_TO_SCREEN);
		}

		namespace c_transform
		{
			inline void(__fastcall *get_position)(uintptr_t self, uintptr_t *pos) = (decltype(get_position))RVA_OFFSET(TRANSFORM_GET_POSITION);
		}

		namespace c_component
		{
			inline uintptr_t(__fastcall* get_transform)(uintptr_t self) = (decltype(get_transform))RVA_OFFSET(COMPONENT_GET_TRANSFORM);
			inline uintptr_t(__fastcall* get_gameObject)(uintptr_t self) = (decltype(get_gameObject))RVA_OFFSET(COMPONENT_GET_GAMEOBJECT);
		}

		namespace c_gameobject
		{
			inline uintptr_t(*Find)(uintptr_t name) = NULL;
			inline uintptr_t(*FindGameObjectsWithTag)(uintptr_t name) = NULL;
		}
	}



	// ----- csharp ------
	//namespace c_sharp
	//{
	//	namespace dictionary
	//	{
	//		inline uint32_t buckets = NULL;
	//		inline uint32_t entries = NULL;
	//		inline uint32_t count = NULL;
	//
	//		namespace c_entry
	//		{
	//			inline uint32_t hashCode = 0x10;//NULL;
	//			inline uint32_t next = 0x14;//NULL;
	//			inline uint32_t key = 0x18;//NULL;
	//			inline uint32_t value = 0x20;//NULL;
	//		}
	//	}
	//
	//	namespace object
	//	{
	//		inline int32_t(__thiscall* GetHashCode)(uintptr_t obj) = NULL;
	//	}
	//}



	static void load_mono()
	{
		mono::c_mono* mono = mono::c_mono::singleton();
		mono_loaded = true;

	#define load_func(ns, tt, func) \
		ns ## :: ## func = (decltype(ns ## :: ## func))mono->get_method_from_class(tt, c(#func));

	#define load_obf_func(ns, tt, func, func_n) \
		ns ## :: ## func = (decltype(ns ## :: ## func))mono->get_method_from_class(tt, c(func_n));

	#define load_offset(ns, tt, off) \
		ns ## :: ## off = (decltype(ns ## :: ## off))mono->get_class_member_offset(tt, c(#off));

	#define load_obf_offset(ns, tt, off, off_n) \
		ns ## :: ## off = (decltype(ns ## :: ## off))mono->get_class_member_offset(tt, c(off_n));

		DEBUG("[offsets]: loading offsets...\n");

		/*uintptr_t iterate = 0;
		while (uintptr_t method = mono->mono_class_get_methods(world, &iterate))
		{
			std::string tname = mono->name_reality_check(mono->mono_method_get_name(method));
			uintptr_t sig = mono->mono_method_signature(method);
			if (!sig)
			{
				DEBUG("c_world::%s: unknown args.\n", tname.c_str());
			}
			else
			{
				DEBUG("c_world::%s: %i args.\n", tname.c_str(), *(uint16_t*)(sig + 8));
			}
		}


		iterate = 0;
		uintptr_t field = 0;
		do
		{
			field = mono->mono_class_get_fields(world, &iterate);
			if (field)
			{
				std::string p = mono->name_reality_check(mono->mono_field_get_name(field));
				DEBUG("c_world::%s: 0x%04X\n", p.c_str(), mono->mono_field_get_offset(field));
			}
		} while (field);*/


		// c_gameworld::*
		{
			uintptr_t gameworld = mono->find_class(c("GameWorld"));
			DEBUG("c_gameworld: 0x%p\n", gameworld);

			if (gameworld)
			{
				load_obf_func(c_gameworld, gameworld, gameworld_ctor, ".ctor");
				DEBUG("c_gameworld::.ctor: 0x%p\n", c_gameworld::gameworld_ctor);
				load_obf_func(c_gameworld, gameworld, gameworld_dispose, "Dispose");
				DEBUG("c_gameworld::Dispose: 0x%p\n", c_gameworld::gameworld_dispose);

				load_offset(c_gameworld, gameworld, RegisteredPlayers);
				DEBUG("c_gameworld::RegisteredPlayers: 0x%04X\n", c_gameworld::RegisteredPlayers);
				load_offset(c_gameworld, gameworld, LootList);
				DEBUG("c_gameworld::LootList: 0x%04X\n", c_gameworld::LootList);

				load_func(c_gameworld, gameworld, get_AllPlayers);
				DEBUG("c_gameworld::get_AllPlayers: 0x%p\n", c_gameworld::get_AllPlayers);
			}
			else
				DEBUG("[offsets]: failed to get GameWorld class.\n");
		}

		// c_player::*
		{
			uintptr_t player = mono->find_class(c("Player"));
			DEBUG("c_player: 0x%p\n", player);

			if (player)
			{
				load_func(c_player, player, get_IsYourPlayer);
				DEBUG("c_player::get_IsYourPlayer: 0x%p\n", c_player::get_IsYourPlayer);
				load_func(c_player, player, get_IsAI);
				DEBUG("c_player::get_IsAI: 0x%p\n", c_player::get_IsAI);
				load_func(c_player, player, get_PlayerBody);
				DEBUG("c_player::get_PlayerBody: 0x%p\n", c_player::get_PlayerBody);

				load_offset(c_player, player, Physical);
				DEBUG("c_player::Physical: 0x%04X\n", c_player::Physical);
			}
			else
				DEBUG("[offsets]: failed to get Player class.\n");
		}

		// c_playerbody::*
		{
			uintptr_t playerbody = mono->find_class(c("PlayerBody"));
			DEBUG("c_playerbody: 0x%p\n", playerbody);

			if (playerbody)
			{
				load_offset(c_playerbody, playerbody, PlayerBones);
				DEBUG("c_playerbody::PlayerBones: 0x%04X\n", c_playerbody::PlayerBones);
				load_offset(c_playerbody, playerbody, _side);
				DEBUG("c_playerbody::_side: 0x%04X\n", c_playerbody::_side);
				load_offset(c_playerbody, playerbody, SkeletonRootJoint);
				DEBUG("c_playerbody::SkeletonRootJoint: 0x%04X\n", c_playerbody::SkeletonRootJoint);
			}
			else
				DEBUG("[offsets]: failed to get PlayerBody class.\n");
		}

		// c_skeleton::*
		{
			uintptr_t skeleton = mono->find_class(c("Skeleton"), "", c("Diz.Skinning"));
			DEBUG("c_skeleton: 0x%p\n", skeleton);

			if (skeleton)
			{
				load_offset(c_skeleton, skeleton, _keys);
				DEBUG("c_skeleton::_values: 0x%04X\n", c_skeleton::_keys);
				load_offset(c_skeleton, skeleton, _values);
				DEBUG("c_skeleton::_values: 0x%04X\n", c_skeleton::_values);
			}
			else
				DEBUG("[offsets]: failed to get Skeleton class.\n");
		}


		// c_ballistics_calculator::*
		{
			uintptr_t ballisticscalculator = mono->find_class(c("BallisticsCalculator"));
			DEBUG("c_ballistics_calculator: 0x%p\n", ballisticscalculator);

			if (ballisticscalculator)
			{
				load_func(c_ballistics_calculator, ballisticscalculator, CreateShot);
				DEBUG("c_ballistics_calculator::CreateShot: 0x%04X\n", c_ballistics_calculator::CreateShot);
			}
			else
				DEBUG("[offsets]: failed to  get Skeleton class.\n");
		}



		// --------- unity offsets ---------
		// unity::c_gameobject
		{
			uintptr_t gameobject = mono->find_class(c("GameObject"), MONO_ALL_ASSEMBLIES);
			DEBUG("c_gameobject: 0x%p\n", gameobject);

			if (gameobject)
			{
				load_func(unity::c_gameobject, gameobject, Find);
				DEBUG("c_gameobject::Find: 0x%p\n", unity::c_gameobject::Find);
				load_func(unity::c_gameobject, gameobject, FindGameObjectsWithTag);
				DEBUG("c_gameobject::FindGameObjectsWithTag: 0x%p\n", unity::c_gameobject::FindGameObjectsWithTag);
			}
			else
				DEBUG("[offsets]: failed to get GameObject class.\n");
		}



		//// --------- csharp offsets ---------
		//// c_sharp::dictionary
		//{
		//	uintptr_t dictionary = mono->find_class(c("Dictionary"), MONO_ALL_ASSEMBLIES, c("System.Collections.Generic"));
		//	DEBUG("c_sharp::dictionary: 0x%p\n", dictionary);
		//
		//	if (dictionary)
		//	{
		//		load_offset(c_sharp::dictionary, dictionary, buckets);
		//		DEBUG("c_sharp::dictionary::GetHashCode: 0x%04X\n", c_sharp::dictionary::buckets);
		//		load_offset(c_sharp::dictionary, dictionary, entries);
		//		DEBUG("c_sharp::dictionary::GetHashCode: 0x%04X\n", c_sharp::dictionary::buckets);
		//		load_offset(c_sharp::dictionary, dictionary, count);
		//		DEBUG("c_sharp::dictionary::GetHashCode: 0x%04X\n", c_sharp::dictionary::buckets);
		//	}
		//	else
		//		DEBUG("[offsets]: failed to get Dictionary class.\n");
		//}
		//
		//// c_sharp::object
		//{
		//	uintptr_t object = mono->find_class(c("Object"), MONO_ALL_ASSEMBLIES, c("System"));
		//	DEBUG("c_sharp::object: 0x%p\n", object);
		//
		//	if (object)
		//	{
		//		load_offset(c_sharp::object, object, GetHashCode);
		//		DEBUG("c_sharp::object::GetHashCode: 0x%p\n", c_sharp::object::GetHashCode);
		//	}
		//	else
		//		DEBUG("[offsets]: failed to get Object class.\n");
		//}



		// --------- offset rebase ---------
	#define set_offset(ns, offset, dll) \
		ns ## :: ## offset = (decltype(ns ## :: ## offset))(((uintptr_t)ns ## :: ## offset) + dll)

		// UnityPlayer.dll
		uintptr_t unity_player = util::get_process_module(c("UnityPlayer.dll"));

		// unity::c_camera
		set_offset(unity::c_camera, get_main, unity_player);
		set_offset(unity::c_camera, WorldToScreenPoint, unity_player);


		// unity::c_transform
		set_offset(unity::c_transform, get_position, unity_player);

		// unity::c_component
		set_offset(unity::c_component, get_transform, unity_player);
		set_offset(unity::c_component, get_gameObject, unity_player);
	}
}