#include "memory_hook.h"
#include <stdexcept>
#include <Windows.h>
#include <string>


#include <util/xor.hpp>
#include <loader/main.h>
#include <util/funcs.h>


#ifdef _WIN64
#include "hde64.h"
#include "table64.h"
#else
#pragma error("32-bit hooks are not supported.")
#include "hde32.h"
#include "table32.h"
#endif



// helpers
//static const uint8_t call_hook_stub[] = { 0x50, 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD0, 0x58 };
//static constexpr uint32_t call_hook_offset = 3;
//static constexpr uint32_t call_hook_offset_tramp = 3;

static const uint8_t relay_hook_stub[] = {
			0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		//mov r10, addr
			0x41, 0xFF, 0xE2										//jmp r10
};
static constexpr uint32_t relay_hook_offset = 2;

static const uint8_t jmp_hook_stub[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
static constexpr uint32_t jmp_hook_offset = 2;



const uintptr_t calc_relative_offset(const uintptr_t from, const uintptr_t to, const uint32_t jmp_size)
{
	if (from > to)
		return 0 - (from - to) - jmp_size;
	return to - (from + jmp_size);
}


const uint32_t calc_nops(const uintptr_t func, const uint32_t nsize, uint32_t fnops = 0)
{
	uintptr_t target = func;
	uint32_t nops = 0;

	while (true)
	{
	#ifdef _WIN64
		hde64s disasm;
		hde64_disasm((void*)target, &disasm);
	#else
		hde32s disasm;
		hde32_disasm((void*)target, &disasm);
	#endif
		target += disasm.len;
		nops += disasm.len;

		if (nops >= nsize)
		{
			if (fnops--)
				continue;
			break;
		}
	}
	nops -= nsize;
	return nops;
}


const uintptr_t follow_hook(const uintptr_t func, const uint32_t nsize, bool *is_hooked)
{
	uintptr_t at = func;
	while ((at - func) <= nsize)
	{
	#ifdef _WIN64
		hde64s disasm;
		hde64_disasm((void*)at, &disasm);
	#else
		hde32s disasm;
		hde32_disasm((void*)at, &disasm);
	#endif

		switch (disasm.opcode)
		{
			case 0xE9: // jmp near, possibly hooked
			{
				if ((disasm.flags & F_IMM32) > 0)
				{
					*is_hooked = true;
					return at + (int32_t)disasm.imm.imm32 + disasm.len;
				}
				break;
			}
			case 0xFF: // jmp near?
			{
				if ((disasm.flags & F_IMM64) > 0)
				{
					*is_hooked = true;
					return disasm.imm.imm64;
				}
				else if (disasm.len == 2)
				{
					MessageBox(NULL, c("MAYBE GOT ONE?!"), c(""), MB_OK);
					DEBUG("bytes: ");
					for (uint32_t i = 0; i < disasm.len; i++)
						DEBUG("%02X ", *(uint8_t*)(at + i));
					DEBUG(", ");
					DEBUG("flags: %04X\n", disasm.flags);
				}
				break;
			}
			case 0x36: // xchg qword ptr ss:[rsp]
			{
				if (disasm.len == 5)
				{
					MessageBox(NULL, c("MAYBE GOT ONE?!"), c(""), MB_OK);
					DEBUG("bytes: ");
					for (uint32_t i = 0; i < disasm.len; i++)
						DEBUG("%02X ", *(uint8_t*)(at + i));
					DEBUG(", ");
					DEBUG("flags: %04X\n", disasm.flags);
				}
				break;
			}
		}

		at += disasm.len;
	}

	// If we make it to here, we're not hooked (well, dunno about that but we can hook at least with *nsize* amount of bytes to write)
	*is_hooked = false;
	return func;
}


size_t calc_length(const uintptr_t func, const size_t max_size)
{
	uintptr_t end = func;
	while (end - func < max_size)
	{
	#ifdef _WIN64
		hde64s disasm;
		hde64_disasm((void*)end, &disasm);
	#else
		hde32s disasm;
		hde32_disasm((void*)end, &disasm);
	#endif
		switch (disasm.opcode)
		{
			case 0xC3: return end - func;
			case 0xCB: return end - func;
			case 0xC2: return end - func;
			case 0xCA: return end - func;
			default: break;
		}

		end += disasm.len;
	}
	return end - func;
}



void fix_relatives(const uintptr_t func, const uintptr_t copy, const size_t size)
{
	uintptr_t at = func;
	while (at - func < size)
	{
	#ifdef _WIN64
		hde64s disasm;
		hde64_disasm((void*)at, &disasm);
	#else
		hde32s disasm;
		hde32_disasm((void*)at, &disasm);
	#endif
		switch (disasm.opcode)
		{
			case 0xE8: // call
			{
				if ((disasm.flags & F_IMM32) > 0)
				{
					uint32_t rel_addr = disasm.imm.imm32;
					uintptr_t o_call = func + (at - copy);
					uintptr_t new_rel = o_call + rel_addr + disasm.len;
					if (new_rel % 10 == 0)
					{
						if (new_rel > (new_rel & 0xFFFFFFFF))
						{
							MessageBox(NULL, c("FAILED REL"), c(""), MB_OK);
							MessageBox(NULL,
								(std::string(c("hook don't know i srry :C\nsend app & info to cybie @ 0x")) +
									std::to_string(func - util::get_process_module("")) +
									":" +
									std::to_string(at - func)).c_str(),
								c("ERR"),
								MB_OK);
						}
						memcpy((void*)(at + 1), &new_rel, sizeof(new_rel));
					}
					else
					{
						MessageBox(NULL,
							c("REL OUT OF LINE"),
							c("ERR"),
							MB_OK);
					}
				}
				else if ((disasm.flags & F_IMM16) > 0)
				{
					MessageBox(NULL,
						(std::string(c("hook don't know i srry :C\nsend app & info to cybie @ 0x")) +
						std::to_string(func - util::get_process_module("")) +
						":" +
						std::to_string(at - func)).c_str(),
						c("ERR"),
						MB_OK);
				}
				break;
			}
			case 0xE9:
			{
				if ((disasm.flags & F_IMM32) > 0)
				{
					uint32_t rel_addr = disasm.imm.imm32;
					uintptr_t o_jmp = func + (at - copy);
					uintptr_t new_rel = o_jmp + rel_addr + disasm.len;
					if (new_rel % 10 == 0)
					{
						if (new_rel > (new_rel & 0xFFFFFFFF))
						{
							MessageBox(NULL, c("FAILED REL"), c(""), MB_OK);
							MessageBox(NULL,
								(std::string(c("hook don't know i srry :C\nsend app & info to cybie @ 0x")) +
									std::to_string(func - util::get_process_module("")) +
									":" +
									std::to_string(at - func)).c_str(),
								c("ERR"),
								MB_OK);
						}
						memcpy((void*)(at + 1), &new_rel, sizeof(new_rel));
					}
					else
					{
						MessageBox(NULL,
							c("REL OUT OF LINE"),
							c("ERR"),
							MB_OK);
					}
				}
				else if ((disasm.flags & F_IMM16) > 0)
				{
					MessageBox(NULL,
						(std::string(c("hook don't know i srry :C\nsend app & info to cybie @ 0x")) +
							std::to_string(func - util::get_process_module("")) +
							":" +
							std::to_string(at - func)).c_str(),
						c("ERR"),
						MB_OK);
				}
				break;
			}
			case 0xFF:
			{
				MessageBox(NULL,
					(std::string(c("hook don't know i srry :C\nsend app & info to cybie @ 0x")) +
						std::to_string(func - util::get_process_module("")) +
						":" +
						std::to_string(at - func)).c_str(),
					c("ERR"),
					MB_OK);
				break;
			}
		}

		at += disasm.len;
	}
}


void memory_hook::memhook_context::build_trampoline()
{
	// calculate the size and store original bytes
	uint32_t nops = calc_nops(this->target, this->payload_size);
	memset(this->hook_payload + this->payload_size, 0x90, nops);

	this->hook_size = this->payload_size + nops;
	this->orig_bytes = (uint8_t*)malloc(this->hook_size);
	if (!this->orig_bytes)
	{
		MessageBox(NULL, c("hooking api cannot allocate [tramp]"), c("ERR:"), MB_OK);
		return;
	}

	DWORD old;
	VirtualProtect((void*)this->target, this->hook_size, PAGE_EXECUTE_READWRITE, &old);
	memcpy(this->orig_bytes, (void*)this->target, this->hook_size);
	VirtualProtect((void*)this->target, this->hook_size, old, &old);


	// create our "original" function
	this->trampoline = (uintptr_t)VirtualAlloc(NULL, this->hook_size + sizeof(relay_hook_stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!this->trampoline)
	{
		MessageBox(NULL, c("hooking api cannot allocate [tramp 2]"), c("ERR:"), MB_OK);
		return;
	}

	this->trampoline_size = this->hook_size + sizeof(relay_hook_stub);

	// copy old function memory
	memcpy((void*)this->trampoline, this->orig_bytes, this->hook_size);

	// jmp back to original code
	uintptr_t relay = this->trampoline + this->hook_size;
	memcpy((void*)relay, (void*)relay_hook_stub, sizeof(relay_hook_stub));

	uintptr_t back_to = this->target + this->hook_size;
	//DEBUG("back to: 0x%p\n", back_to);
	memcpy((void*)(relay + 2), (void*)&back_to, sizeof(uintptr_t));
	fix_relatives(this->target, this->trampoline, this->trampoline_size - sizeof(relay_hook_stub));
}



// memory_hook::memhook_context::*
const uintptr_t memory_hook::memhook_context::create_hook(const uintptr_t detour, const bool follow)//, const bool event_hook)
{
	if (follow)
	{
		/* hooks generally aren't bigger than 0x18 (24) bytes */
		bool is_hooked = false;
		do
		{
			this->target = follow_hook(this->target, 0x18, &is_hooked);
		} while (is_hooked && this->target);

		if (!this->target)
			return NULL;
	}

	const size_t max_size = /*event_hook ? sizeof(call_hook_stub) :*/ sizeof(relay_hook_stub);
	size_t freedom = calc_length(this->target, max_size); // freedom bytes :)
	bool is_relative = false;

	//if (event_hook)
	//{
	//	this->payload_size = max_size;
	//	this->hook_payload = (uint8_t*)malloc(max_size);
	//	if (!this->hook_payload)
	//	{
	//		this->payload_size = 0;
	//		MessageBox(NULL, c("hooking api cannot allocate"), c("ERR:"), MB_OK);
	//		return NULL;
	//	}
	//
	//	memcpy(this->hook_payload, call_hook_stub, max_size);
	//	memcpy((void*)(this->hook_payload + call_hook_offset), &detour, sizeof(uintptr_t));
	//	is_relative = false;
	//	this->hook_type = memhook_type::event_hook;
	//}
	//else if (freedom < max_size)
	if (freedom < max_size)
	{
		// try relative jmp
		this->payload_size = sizeof(jmp_hook_stub);
		uintptr_t rel_off = calc_relative_offset(this->target, this->detour, this->payload_size);
		if (rel_off != (rel_off & 0xFFFFFFFF))
		{
			MessageBox(NULL, c("hooking api cannot hook [rel, off2big, smallpp]"), c("ERR:"), MB_OK);
			return NULL;
		}

		this->hook_payload = (uint8_t*)malloc(this->payload_size);
		if (!this->hook_payload)
		{
			this->payload_size = 0;
			MessageBox(NULL, c("hooking api cannot allocate"), c("ERR:"), MB_OK);
			return NULL;
		}

		memcpy(this->hook_payload, jmp_hook_stub, this->payload_size);

		uint32_t rel_32 = rel_off & 0xFFFFFFFF;
		memcpy((void*)(hook_payload + jmp_hook_offset), &rel_32, sizeof(rel_32));
		is_relative = true;
		this->hook_type = memhook_type::jmp_hook;
	}
	else
	{

		this->payload_size = max_size;
		this->hook_payload = (uint8_t*)malloc(max_size);
		if (!this->hook_payload)
		{
			this->payload_size = 0;
			MessageBox(NULL, c("hooking api cannot allocate"), c("ERR:"), MB_OK);
			return NULL;
		}

		memcpy(this->hook_payload, relay_hook_stub, max_size);
		memcpy((void*)(this->hook_payload + relay_hook_offset), &detour, sizeof(uintptr_t));
		is_relative = false;
		this->hook_type = memhook_type::relay_hook;
	}

	this->detour = detour;
	this->build_trampoline();
	return this->trampoline;
}

void memory_hook::memhook_context::destroy_hook()
{
	if (!this->target || !this->hook_size || !this->hook_payload)
		return;
	disable_hook();

	if (this->hook_payload) free(this->hook_payload);
	if (this->orig_bytes) free(this->orig_bytes);
	if (this->trampoline) VirtualFree((void*)this->trampoline, NULL, MEM_RELEASE);
	this->hook_payload = NULL;
}

void memory_hook::memhook_context::enable_hook()
{
	if (!this->target || !this->detour || !this->hook_payload || !this->hook_size)
		return;

	// copy our payload
	DWORD old;
	VirtualProtect((void*)this->target, this->hook_size, PAGE_EXECUTE_READWRITE, &old);
	memcpy((void*)this->target, (void*)this->hook_payload, this->hook_size);
	VirtualProtect((void*)this->target, this->hook_size, old, &old);
}

void memory_hook::memhook_context::disable_hook()
{
	if (!this->target || !this->detour || !this->orig_bytes || !this->hook_size)
		return;

	// copy original bytes
	DWORD old;
	VirtualProtect((void*)this->target, this->hook_size, PAGE_EXECUTE_READWRITE, &old);
	memcpy((void*)this->target, (void*)this->orig_bytes, this->hook_size);
	VirtualProtect((void*)this->target, this->hook_size, old, &old);
}



// memory_hook::*
std::list<memory_hook::memhook_context*> hook_contexts;

memory_hook::memhook_context* memory_hook::create_context(const uintptr_t target)
{
	memhook_context* hctx = new memhook_context();
	hctx->target = target;
	return hctx;
}

void memory_hook::destroy_context(const uintptr_t target)
{
	for (memory_hook::memhook_context* hctx : hook_contexts)
		destroy_context(hctx);
}

void memory_hook::destroy_context(memhook_context* hctx)
{
	if (!hctx) return;
	hctx->destroy_hook();
	delete hctx;
}
