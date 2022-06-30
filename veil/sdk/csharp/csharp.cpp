#include "csharp.h"
#include <mono/mono.h>
#include <codecvt>



namespace csharp
{
	// csharp::cstring
	cstring::cstring(const cstring& str)
	{
		this->self = str.self;
	}


	cstring::cstring(const char* str)
	{
		mono::c_mono* mono = mono::c_mono::singleton();
		this->self = (structure::cs_string*)mono->mono_string_new_len(mono->mono_get_root_domain(),
			str, strlen(str));
	}

	cstring::cstring(const std::string& str)
	{
		mono::c_mono* mono = mono::c_mono::singleton();
		this->self = (structure::cs_string*)mono->mono_string_new_len(mono->mono_get_root_domain(),
			str.c_str(), str.size());
	}


	std::string cstring::c_str()
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(std::wstring((wchar_t*)&self->chars, self->length));
	}
}