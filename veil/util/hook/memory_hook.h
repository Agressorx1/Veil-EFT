#ifndef MEMORY_HOOK_H
#define MEMORY_HOOK_H
#pragma once
#include <stdint.h>
#include <list>
/*
	x86/x64 (32-bit and 64-bit compatible)
	Memory Hooking Api by Cyberhound#5672 (discord)

		It was a long long day. I have troubles hearing. I thought I saw a plane,
	but turns out it was just a dolphin in the sky, and I thought to myself, "Hell!
	I wish I could fly like a dolphin too." Then one day, I just woke up and flew away!


	I'm retarded, so enjoy this uh- artwork? of C++ madness episode 4 of season 99.
	C++ is just beautiful.


	Note:
		I did not use classes, because they give off RTTI and can be sigged/detectable that way,
	at least I assume so. Though, structs should be safe as they do not have names.
*/


namespace memory_hook
{
	enum class memhook_type
	{
		jmp_hook,
		relay_hook,
		//event_hook
	};

	struct memhook_context
	{
		uintptr_t target;
		uintptr_t detour;

		memhook_type hook_type;


		uint8_t* orig_bytes;
		uint8_t* hook_payload;
		uint32_t payload_size;

		uint32_t hook_size;

		uintptr_t trampoline;
		uint32_t trampoline_size;


		const uintptr_t create_hook(const uintptr_t detour, const bool follow_hook = false);//, const bool event_hook = false);
		void destroy_hook();

		void build_trampoline();

		void enable_hook();
		void disable_hook();

		void toggle_hook(const bool value) { if (value) enable_hook(); else disable_hook(); }


		template<typename detour_T>
		inline const detour_T hook(detour_T detour, const bool follow = true/*, const bool event_hook = false*/) { return (detour_T)create_hook((uintptr_t)detour, follow/*, event_hook*/); }

		template<typename target_T, typename detour_T>
		inline const target_T hook(detour_T detour, const bool follow = true/*, const bool event_hook = false*/) { return (target_T)create_hook((uintptr_t)detour, follow/*, event_hook*/); }
	};


	inline std::list<memhook_context *> hook_list = std::list<memhook_context*>();


	memhook_context *create_context(const uintptr_t target);
	void destroy_context(const uintptr_t target);
	void destroy_context(memhook_context* hctx);

	static void destroy_all_contexts()
	{
		for (memhook_context* hctx : hook_list)
			destroy_context(hctx);
	}


	template<typename target_T, typename detour_T>
	target_T create_hook(const target_T& target, const detour_T& detour) { return create_context((uintptr_t)target)->hook((uintptr_t)detour); }

	template<typename target_T>
	memhook_context *create_context(const target_T& target) { return create_context((uintptr_t)target); }
}
#endif // MEMORY_HOOK_H