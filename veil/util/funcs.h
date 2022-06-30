#pragma once
#include <stdint.h>
#include <locale>
#include <codecvt>
#include <string>


namespace util
{
	uintptr_t get_process_module(const std::wstring& module_name);
	static uintptr_t get_process_module(const std::string& module_name)
	{
		if (!module_name.empty())
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			return get_process_module(converter.from_bytes(module_name));
		}
		return get_process_module("");
	}

	uintptr_t get_proc_address(const uintptr_t module_base, const std::string& proc_name, bool case_insensitive = false);
}