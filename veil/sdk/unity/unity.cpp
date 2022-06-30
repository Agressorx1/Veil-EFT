#include <render/render.h>
#include "unity.h"


namespace unity
{
	// unity::camera
	camera camera::main_camera()
	{
		return functions::unity::c_camera::get_main();
	}


	vector3f camera::world_to_screen(const vector3f& pos)
	{
		if (!self) return vector3f();
		vector3f screen_pos;
		functions::unity::c_camera::WorldToScreenPoint(this->self, &pos, MonoOrStereoscopicEye::Mono, &screen_pos);
		if (screen_pos.y != 0)
			screen_pos.y = render::window.height - screen_pos.y;
		return screen_pos;
	}

	bool camera::world_to_screen(const vector3f& pos, vector3f& screen_out)
	{
		if (!self) return false;

		vector3f scr = world_to_screen(pos);
		if (scr.z <= 0) return false;

		screen_out = scr;
		return true;
	}


	vector3f camera::world_to_screen_from_point(const vector3f& pos, const vector3f& point)
	{
		vector3f screen_pos = world_to_screen(pos);
		return vector3f(fabs(screen_pos.x - point.x), fabs(screen_pos.y - point.y), screen_pos.z);
	}



	// unity::transform
	vector3f transform::position()
	{
		if (!self) return vector3f();
		vector3f pos;
		functions::unity::c_transform::get_position(self, (uintptr_t*)&pos);
		return pos;
	}


	// unity::component
	transform component::get_transform(uintptr_t inst)
	{
		if (!inst) return NULL;
		return functions::unity::c_component::get_transform(inst);
	}


	transform component::transform()
	{
		if (!self) return NULL;
		return functions::unity::c_component::get_transform(this->self);
	}


	// unity::game_object
	game_object game_object::find_object(const csharp::cstring& name)
	{
		return functions::unity::c_gameobject::Find(name);
	}

	csharp::carray<game_object> game_object::find_objects(const csharp::cstring& name)
	{
		return functions::unity::c_gameobject::FindGameObjectsWithTag(name);
	}
}