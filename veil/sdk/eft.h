#pragma once
#include <stdint.h>
#include <list>
#include <string>

#include <sdk/game/offsets.hpp>
#include <sdk/instance.h>
#include <util/funcs.h>

#include <mono/mono.h>

// additional sdk
#include "csharp/csharp.h"
#include "unity/unity.h"



namespace eft
{
	enum class PlayerSide
	{
		Usec = 1,
		Bear,
		Savage = 4,
		Unknown
	};

	enum class PlayerBoneId
	{
		BaseHuman = 0,
		IKController = 1,
		Mesh = 2,
		Vest_0 = 3,
		Vest_1 = 4,
		backpack = 5,
		backpack_backpack_0 = 6,
		backpack_backpack_1 = 7,
		backpack_backpack_2 = 8,
		razgruz = 9,
		razgruz_razgruz_0 = 10,
		razgruz_razgruz_1 = 11,
		razgruz_razgruz_2 = 12,
		Root_Joint = 13,
		HumanPelvis = 14,
		HumanLThigh1 = 15,
		HumanLThigh2 = 16,
		HumanLCalf = 17,
		HumanLFoot = 18,
		HumanLToe = 19,
		HumanRThigh1 = 20,
		HumanRThigh2 = 21,
		HumanRCalf = 22,
		HumanRFoot = 23,
		HumanRToe = 24,
		HumanRThigh1_Bear_Feet = 25,
		HumanRThigh1_USEC_Feet = 26,
		HumanRThigh1_BEAR_feet_1 = 27,
		HumanRThigh1_weapon_holster_pistol = 28,
		HumanSpine1 = 29,
		HumanGear1 = 30,
		HumanGear2 = 31,
		HumanGear3 = 32,
		HumanGear4 = 33,
		HumanGear4_1 = 34,
		HumanGear5 = 35,
		HumanSpine2 = 36,
		HumanSpine3 = 37,
		IK_S_LForearm1 = 38,
		IK_S_LForearm2 = 39,
		IK_S_LForearm3 = 40,
		IK_S_LPalm = 41,
		IK_S_LDigit11 = 42,
		IK_S_LDigit12 = 43,
		IK_S_LDigit13 = 44,
		IK_S_LDigit21 = 45,
		IK_S_LDigit22 = 46,
		IK_S_LDigit23 = 47,
		IK_S_LDigit31 = 48,
		IK_S_LDigit32 = 49,
		IK_S_LDigit33 = 50,
		IK_S_LDigit41 = 51,
		IK_S_LDigit42 = 52,
		IK_S_LDigit43 = 53,
		IK_S_LDigit51 = 54,
		IK_S_LDigit52 = 55,
		IK_S_LDigit53 = 56,
		IK_S_LPalm_XYZ = 57,
		LCollarbone_anim = 58,
		RCollarbone_anim = 59,
		RCollarbone_anim_XYZ = 60,
		Weapon_root_3rd_anim = 61,
		Weapon_root_3rd_anim_XYZ_1 = 62,
		Bend_Goal_Left = 63,
		Bend_Goal_Right = 64,
		Bend_Goal_Right_XYZ_1 = 65,
		HumanRibcage = 66,
		IK_LForearm1 = 67,
		IK_LForearm2 = 68,
		IK_LForearm3 = 69,
		IK_LPalm = 70,
		IK_LDigit11 = 71,
		IK_LDigit12 = 72,
		IK_LDigit13 = 73,
		IK_LDigit21 = 74,
		IK_LDigit22 = 75,
		IK_LDigit23 = 76,
		IK_LDigit31 = 77,
		IK_LDigit32 = 78,
		IK_LDigit33 = 79,
		IK_LDigit41 = 80,
		IK_LDigit42 = 81,
		IK_LDigit43 = 82,
		IK_LDigit51 = 83,
		IK_LDigit52 = 84,
		IK_LDigit53 = 85,
		Camera_animated = 86,
		CameraContainer = 87,
		Cam = 88,
		HumanLCollarbone = 89,
		HumanLUpperarm = 90,
		HumanLForearm1 = 91,
		HumanLForearm2 = 92,
		HumanLForearm3 = 93,
		HumanLPalm = 94,
		HumanLDigit11 = 95,
		HumanLDigit12 = 96,
		HumanLDigit13 = 97,
		HumanLDigit21 = 98,
		HumanLDigit22 = 99,
		HumanLDigit23 = 100,
		HumanLDigit31 = 101,
		HumanLDigit32 = 102,
		HumanLDigit33 = 103,
		HumanLDigit41 = 104,
		HumanLDigit42 = 105,
		HumanLDigit43 = 106,
		HumanLDigit51 = 107,
		HumanLDigit52 = 108,
		HumanLDigit53 = 109,
		HumanRCollarbone = 110,
		HumanRUpperarm = 111,
		HumanRForearm1 = 112,
		HumanRForearm2 = 113,
		HumanRForearm3 = 114,
		HumanRPalm = 115,
		HumanRDigit11 = 116,
		HumanRDigit12 = 117,
		HumanRDigit13 = 118,
		HumanRDigit21 = 119,
		HumanRDigit22 = 120,
		HumanRDigit23 = 121,
		HumanRDigit31 = 122,
		HumanRDigit32 = 123,
		HumanRDigit33 = 124,
		HumanRDigit41 = 125,
		HumanRDigit42 = 126,
		HumanRDigit43 = 127,
		HumanRDigit51 = 128,
		HumanRDigit52 = 129,
		HumanRDigit53 = 130,
		Weapon_root = 131,
		HumanNeck = 132,
		HumanHead = 133,
		HumanBackpack = 134,
		weapon_holster = 135,
		weapon_holster1 = 136,
		Camera_animated_3rd = 137
	};


	class player_physical : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		player_physical() {}
		~player_physical() {}
	};

	class player_bones : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		player_bones() {}
		~player_bones() {}

	public:
		
	};

	class player_skeleton : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		player_skeleton() {}
		~player_skeleton() {}

	public:
		csharp::clist<csharp::cstring> bone_names();
		csharp::clist<unity::transform> bone_transforms();
	};

	class player_body : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		player_body() {}
		~player_body() {}

	public:
		player_bones bones();
		PlayerSide side();
		player_skeleton root_skeleton();
	};

	class player : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		player() {}
		~player() {}

	public:
		const bool is_local();
		const bool is_AI();

		player_physical physical();
		player_body body();


	public: // quick accessers
		vector3f position()
		{
			if (!self) return NULL;
			return bone(eft::PlayerBoneId::BaseHuman).position();
		}

		player_skeleton skeleton() { return this->body().root_skeleton(); }
		csharp::clist<unity::transform> bones() { return this->skeleton().bone_transforms(); }

		unity::transform bone(PlayerBoneId bone_id)
		{
			if (!self) return NULL;

			csharp::clist<unity::transform> bones = this->bones();
			if (!bones) return NULL;

			int32_t idx = (int32_t)bone_id;
			if (idx < 0 || idx > bones.size())
				return NULL;

			return bones[idx];
		}
	};

	class game_world : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		game_world() {}
		~game_world() {}

	public: // functions
		csharp::clist<player> players();
		csharp::clist<uintptr_t> lootlist();
	};
}



namespace cheat
{
	inline eft::game_world game_world = NULL;

	inline eft::player local_player = NULL;

	inline unity::camera camera = NULL;
	inline unity::camera optical_camera = NULL;


	void update_loop();
}