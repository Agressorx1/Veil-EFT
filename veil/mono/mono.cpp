#include "mono.h"
#include <Windows.h>


mono::c_mono* g_mono;

namespace mono
{
	typedef _MonoMethodSignature MonoMethodSignature;


	// global mono functions
	c_mono* c_mono::singleton()
	{
		if (!g_mono)
		{
			g_mono = new c_mono();

		#define fuck_visual_studio_because_remove_this_and_it_doesnt_know_how_to_syntax(x) #x
		#define lmf(func) \
			g_mono-> ## func = mono_data::create_function<decltype(g_mono-> ## func)>( fuck_visual_studio_because_remove_this_and_it_doesnt_know_how_to_syntax(func) ); \
			if (!g_mono-> ## func) DEBUG("[%s]: had a fault, and was not imported.\n", #func);

			//----- BEGIN OF RAW MONO FUNCS

			lmf(mono_get_root_domain);
			lmf(mono_thread_attach);
			lmf(mono_thread_detach);
			lmf(mono_domain_assembly_open);
			lmf(mono_assembly_get_image);
			lmf(mono_image_get_table_rows);
			lmf(mono_class_get);
			lmf(mono_class_get_name);
			lmf(mono_class_get_method_from_name);
			lmf(mono_compile_method);
			lmf(mono_class_get_methods);
			lmf(mono_method_get_name);
			lmf(mono_class_from_name);
			lmf(mono_class_get_fields);
			lmf(mono_field_get_name);
			lmf(mono_field_get_offset);
			lmf(mono_class_get_type);
			lmf(mono_assembly_foreach);
			lmf(mono_method_signature);
			lmf(mono_class_get_namespace);
			//lmf(mono_method_get_unmanaged_thunk);
			//lmf(mono_runtime_invoke);

			lmf(mono_type_get_object);

			//lmf(mono_object_unbox);

			lmf(mono_string_new_len);

			//----- END OF RAW MONO FUNCS

		#undef lmf
		#undef fuck_visual_studio_because_remove_this_and_it_doesnt_know_how_to_syntax
		}
		g_mono->attach();
		return g_mono;
	}

	void c_mono::cleanup()
	{
		if (!g_mono)
			return;

		// detach all threads
		for (std::pair<uint32_t, uintptr_t> t : g_mono->mono_threads)
			g_mono->mono_thread_detach(t.second);

		g_mono->mono_threads.clear();

		// delete singleton
		delete g_mono;
		g_mono = nullptr;


		mono_data::mono_dll = NULL;
		mono_data::mono_cached.clear();
	}



	// mono interaction
	uintptr_t c_mono::get_obfuscated_class(const std::string& klass_name, const std::string& assembly_name)
	{
		// get assembly & image
		std::list<uintptr_t> targets;
		if (!assembly_name.compare(MONO_ALL_ASSEMBLIES))
			this->open_all_assemblies(targets);
		else
		{
			uintptr_t assembly = default_assembly(assembly_name);
			if (!assembly) return 0;
			targets.push_back(assembly);
		}

		for (uintptr_t assembly : targets)
		{
			uintptr_t image = mono_assembly_get_image(assembly);
			if (!image) return 0;

			// iterate typedef table
			for (int32_t i = 1; i <= mono_image_get_table_rows(image, MONO_TABLE_TYPEDEF); i++)
			{
				uintptr_t tdef = mono_class_get(image, MONO_TOKEN_TYPE_DEF | i);
				if (!name_reality_check(mono_class_get_name(tdef)).compare(klass_name))
					return tdef;
			}
		}
		return 0;
	}

	uintptr_t c_mono::find_class(const std::string& klass_name, const std::string& assembly_name, const std::string& ns)
	{
		if (klass_name.empty()) return 0;

		// get assembly & image
		std::list<uintptr_t> targets;
		if (!assembly_name.compare(MONO_ALL_ASSEMBLIES))
			this->open_all_assemblies(targets);
		else
		{
			uintptr_t assembly = default_assembly(assembly_name);
			if (!assembly) return 0;
			targets.push_back(assembly);
		}
		

		for (uintptr_t assembly : targets)
		{
			if (!assembly) continue;

			uintptr_t image = mono_assembly_get_image(assembly);
			if (!image) return 0;

			// iterate typedef table
			int32_t typedef_count = mono_image_get_table_rows(image, MONO_TABLE_TYPEDEF);
			if (typedef_count <= 0)
				continue;

			for (int32_t i = 1; i <= typedef_count; i++)
			{
				uintptr_t tdef = mono_class_get(image, MONO_TOKEN_TYPE_DEF | i);
				if (!tdef) continue;

				std::string tname = name_reality_check(mono_class_get_name(tdef));
				if (!tname.compare(klass_name))
				{
					if (!ns.empty())
					{
						// namespace checking
						if (!name_reality_check(mono_class_get_namespace(tdef)).compare(ns))
							return tdef;
					}
					else
						return tdef;
				}
			}
		}
		return 0;
	}

	
	void sanitize_method_name(const std::string& name, std::string& ns, std::string& klass, std::string& method)
	{
		if (auto ns_break = name.find(c("::")))
			ns = name.substr(0, ns_break);

		if (name.find(c("::")) == std::string::npos)
			ns = "";

		if (name.find(c("::")))
		{
			klass = name.substr(name.find(c("::"), 2) + 2);
			klass = klass.substr(0, klass.find(c("."), 0));
		}

		if (name.find(c("::")) == std::string::npos)
			klass = name.substr(0, name.find(c("."), 0));

		method = name.substr(name.find_last_of(c(".")) + 1);

		if (method.find(c("()")))
			method.erase(method.find(c("()")), 2);
	}


	uintptr_t c_mono::get_class_filtered(const std::string& name_space, const std::string& klass_name, const std::string& assembly_name)
	{
		if (klass_name.empty()) return 0;

		// get assembly & image
		std::list<uintptr_t> targets;
		if (!assembly_name.compare(MONO_ALL_ASSEMBLIES))
			this->open_all_assemblies(targets);
		else
		{
			uintptr_t assembly = default_assembly(assembly_name);
			if (!assembly) return 0;
			targets.push_back(assembly);
		}


		for (uintptr_t assembly : targets)
		{
			uintptr_t image = mono_assembly_get_image(assembly);
			if (!image) return 0;

			// iterate typedef table
			for (int32_t i = 1; i <= mono_image_get_table_rows(image, MONO_TABLE_TYPEDEF); i++)
			{
				uintptr_t tdef = mono_class_get(image, MONO_TOKEN_TYPE_DEF | i);
				if (!tdef) continue;

				std::string tname = name_reality_check(mono_class_get_name(tdef));
				if (!tname.compare(klass_name) && !name_space.compare(mono_class_get_namespace(tdef)))
					return tdef;
			}
		}
		return 0;
	}

	uintptr_t c_mono::get_method_from_class(uintptr_t klass, const std::string& method_name, int arg_count)
	{
		if (!klass) return 0;

		uintptr_t iterate = 0;
		uintptr_t last_method = 0;
		while (uintptr_t method = mono_class_get_methods(klass, &iterate))
		{
			std::string tname = name_reality_check(mono_method_get_name(method));
			if (!tname.compare(method_name))
			{
				if (arg_count == -1)
					return mono_compile_method(method);
				else
				{
					uintptr_t sig = mono_method_signature(method);
					if (!sig)
						last_method = method;
					if (*(uint16_t*)(sig + 8) == arg_count)
						return mono_compile_method(method);
				}
			}
		}
		return last_method ? mono_compile_method(last_method) : 0;
	}

	uintptr_t c_mono::get_method(const std::string& name_string, const std::string& assembly_name)
	{
		std::string namespace_n = "", klass_n = "", method_n = "";
		sanitize_method_name(name_string, namespace_n, klass_n, method_n);

		uintptr_t klass = find_class(klass_n, assembly_name);
		if (!klass) return 0;

		uintptr_t iterate;
		while (uintptr_t method = mono_class_get_methods(klass, &iterate))
		{
			std::string tname = name_reality_check(mono_method_get_name(method));
			if (!tname.compare(method_n))
				return mono_compile_method(method);
		}
		return 0;
	}
	
	
	//uintptr_t c_mono::get_method_from_class_unmanaged(uintptr_t klass, const std::string& method_name, int arg_count)
	//{
	//	if (!klass) return 0;
	//
	//	uintptr_t iterate{};
	//	uintptr_t last_method = 0;
	//	while (uintptr_t method = mono_class_get_methods(klass, &iterate))
	//	{
	//		std::string tname = name_reality_check(mono_method_get_name(method));
	//		if (!tname.compare(method_name))
	//		{
	//			if (arg_count == -1)
	//				return mono_method_get_unmanaged_thunk(method);
	//			else
	//			{
	//				uintptr_t sig = mono_method_signature(method);
	//				if (!sig)
	//					last_method = method;
	//				if (*(uint16_t*)(sig + 8) == arg_count)
	//					return mono_method_get_unmanaged_thunk(method);
	//			}
	//		}
	//	}
	//	return last_method;
	//} 


	uintptr_t c_mono::typeof(uintptr_t klass)
	{
		if (!klass) return 0;
		return mono_type_get_object(mono_get_root_domain(), mono_class_get_type(klass));
	}


	uint32_t c_mono::get_class_member_offset(uintptr_t klass, const std::string& member_name)
	{
		if (!klass) return 0;

		uintptr_t iterate = 0;
		uintptr_t field = 0;
		do
		{
			field = mono_class_get_fields(klass, &iterate);
			if (field)
			{
				std::string p = name_reality_check(mono_field_get_name(field));
				if (!member_name.compare(p))
					return mono_field_get_offset(field);
			}
		} while (field);
		return 0;
	}



	MonoCallConvention c_mono::get_method_convention(uintptr_t klass, const std::string& method_name)
	{
		if (!klass) return MonoCallConvention::MONO_CALL_UNMANAGED_MD;

		uintptr_t iterate = 0;
		uintptr_t last_method = 0;
		uintptr_t method = 0;
		uintptr_t _sig = 0;
		while (method = mono_class_get_methods(klass, &iterate))
		{
			std::string tname = name_reality_check(mono_method_get_name(method));
			if (!tname.compare(method_name))
			{
				_sig = mono_method_signature(method);
				if (!_sig)
				{
					last_method = method;
					continue;
				}
				break;
			}
		}
		
		if (!_sig) return MonoCallConvention::MONO_CALL_UNMANAGED_MD;

		MonoMethodSignature* sig = (MonoMethodSignature*)_sig;
		if (!sig) return MonoCallConvention::MONO_CALL_C;
		switch (sig->call_convention)
		{
			case MonoCallConvention::MONO_CALL_DEFAULT:
				return MonoCallConvention::MONO_CALL_C;
			case MonoCallConvention::MONO_CALL_UNMANAGED_MD:
				return MonoCallConvention::MONO_CALL_C;
			case MonoCallConvention::MONO_CALL_VARARG:
				return MonoCallConvention::MONO_CALL_C;
			default:
				break;
		}
		return (MonoCallConvention)sig->call_convention;
	}



	void c_mono::attach()
	{
		uint32_t id = GetCurrentThreadId();
		if (mono_threads.find(id) != mono_threads.end())
			return;

		mono_threads[id] = mono_thread_attach(mono_get_root_domain());
	}

	void c_mono::detach()
	{
		uint32_t id = GetCurrentThreadId();

		decltype(mono_threads)::iterator it = mono_threads.begin();
		while (it != this->mono_threads.end())
		{
			if (it->first == id)
			{
				mono_thread_detach(it->second);
				break;
			}
			it++;
		}
	}
}