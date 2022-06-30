#pragma once
#include <stdint.h>
#include <list>
#include <string>

#include <sdk/game/offsets.hpp>
#include <sdk/csharp/csharp.h>
#include <sdk/instance.h>
#include <util/funcs.h>

#include <mono/mono.h>


#include "unity_structs.h"


namespace unity
{
	class camera : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		camera() {}
		~camera() {}
	
	public:
		static camera main_camera();
	
	public:
		vector3f world_to_screen(const vector3f& pos);
		bool world_to_screen(const vector3f& pos, vector3f& screen_pos);

		vector3f world_to_screen_from_point(const vector3f& pos, const vector3f& point);
	};



	class transform : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		transform() {}
		~transform() {}


	public:
		vector3f position();
	};


	class component : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		component() {}
		~component() {}

		
	public:
		static unity::transform get_transform(uintptr_t inst);
		

	public: // functions
		unity::transform transform();
	};


	class game_object : public c_instance<uintptr_t>
	{
	public:
		c_instance_import;
		game_object() {}
		~game_object() {}

	public:
		static game_object find_object(const csharp::cstring& name);
		static csharp::carray<game_object> find_objects(const csharp::cstring& name);
	};
}