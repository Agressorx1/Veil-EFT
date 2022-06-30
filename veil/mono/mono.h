#pragma once
#include <stdint.h>
#include <unordered_map>
#include <string>
#include <unordered_map>

#include <loader/main.h>

#include <util/funcs.h>
#include <util/xor.hpp>


#define MONO_DLL_MODULE L"mono-2.0-bdwgc.dll"
#define ASSUMED_DEFAULT_MONO_ASSEMBLY "Assembly-CSharp"


namespace mono
{
	struct _MonoMethodSignature {
		uintptr_t* ret;
	#ifdef MONO_SMALL_CONFIG
		guint8        param_count;
		gint8         sentinelpos;
		unsigned int  generic_param_count : 5;
	#else
		uint16_t       param_count;
		int16_t        sentinelpos;
		unsigned int  generic_param_count : 16;
	#endif
		unsigned int  call_convention : 6;
		unsigned int  hasthis : 1;
		unsigned int  explicit_this : 1;
		unsigned int  pinvoke : 1;
		unsigned int  is_inflated : 1;
		unsigned int  has_type_parameters : 1;
		unsigned int  suppress_gc_transition : 1;
		uintptr_t* params[1];
	};


	typedef enum
	{
		MONO_TOKEN_MODULE = 0x00000000,
		MONO_TOKEN_TYPE_REF = 0x01000000,
		MONO_TOKEN_TYPE_DEF = 0x02000000,
		MONO_TOKEN_FIELD_DEF = 0x04000000,
		MONO_TOKEN_METHOD_DEF = 0x06000000,
		MONO_TOKEN_PARAM_DEF = 0x08000000,
		MONO_TOKEN_INTERFACE_IMPL = 0x09000000,
		MONO_TOKEN_MEMBER_REF = 0x0a000000,
		MONO_TOKEN_CUSTOM_ATTRIBUTE = 0x0c000000,
		MONO_TOKEN_PERMISSION = 0x0e000000,
		MONO_TOKEN_SIGNATURE = 0x11000000,
		MONO_TOKEN_EVENT = 0x14000000,
		MONO_TOKEN_PROPERTY = 0x17000000,
		MONO_TOKEN_MODULE_REF = 0x1a000000,
		MONO_TOKEN_TYPE_SPEC = 0x1b000000,
		MONO_TOKEN_ASSEMBLY = 0x20000000,
		MONO_TOKEN_ASSEMBLY_REF = 0x23000000,
		MONO_TOKEN_FILE = 0x26000000,
		MONO_TOKEN_EXPORTED_TYPE = 0x27000000,
		MONO_TOKEN_MANIFEST_RESOURCE = 0x28000000,
		MONO_TOKEN_GENERIC_PARAM = 0x2a000000,
		MONO_TOKEN_METHOD_SPEC = 0x2b000000,

		/*
		 * These do not match metadata tables directly
		 */
		 MONO_TOKEN_STRING = 0x70000000,
		 MONO_TOKEN_NAME = 0x71000000,
		 MONO_TOKEN_BASE_TYPE = 0x72000000
	} MonoTokenType;


	typedef enum
	{
		MONO_TABLE_MODULE,
		MONO_TABLE_TYPEREF,
		MONO_TABLE_TYPEDEF,
		MONO_TABLE_FIELD_POINTER,
		MONO_TABLE_FIELD,
		MONO_TABLE_METHOD_POINTER,
		MONO_TABLE_METHOD,
		MONO_TABLE_PARAM_POINTER,
		MONO_TABLE_PARAM,
		MONO_TABLE_INTERFACEIMPL,
		MONO_TABLE_MEMBERREF, /* 0xa */
		MONO_TABLE_CONSTANT,
		MONO_TABLE_CUSTOMATTRIBUTE,
		MONO_TABLE_FIELDMARSHAL,
		MONO_TABLE_DECLSECURITY,
		MONO_TABLE_CLASSLAYOUT,
		MONO_TABLE_FIELDLAYOUT, /* 0x10 */
		MONO_TABLE_STANDALONESIG,
		MONO_TABLE_EVENTMAP,
		MONO_TABLE_EVENT_POINTER,
		MONO_TABLE_EVENT,
		MONO_TABLE_PROPERTYMAP,
		MONO_TABLE_PROPERTY_POINTER,
		MONO_TABLE_PROPERTY,
		MONO_TABLE_METHODSEMANTICS,
		MONO_TABLE_METHODIMPL,
		MONO_TABLE_MODULEREF, /* 0x1a */
		MONO_TABLE_TYPESPEC,
		MONO_TABLE_IMPLMAP,
		MONO_TABLE_FIELDRVA,
		MONO_TABLE_ENCLOG,
		MONO_TABLE_ENCMAP,
		MONO_TABLE_ASSEMBLY, /* 0x20 */
		MONO_TABLE_ASSEMBLYPROCESSOR,
		MONO_TABLE_ASSEMBLYOS,
		MONO_TABLE_ASSEMBLYREF,
		MONO_TABLE_ASSEMBLYREFPROCESSOR,
		MONO_TABLE_ASSEMBLYREFOS,
		MONO_TABLE_FILE,
		MONO_TABLE_EXPORTEDTYPE,
		MONO_TABLE_MANIFESTRESOURCE,
		MONO_TABLE_NESTEDCLASS,
		MONO_TABLE_GENERICPARAM, /* 0x2a */
		MONO_TABLE_METHODSPEC,
		MONO_TABLE_GENERICPARAMCONSTRAINT,
		MONO_TABLE_UNUSED8,
		MONO_TABLE_UNUSED9,
		MONO_TABLE_UNUSED10,
		/* Portable PDB tables */
		MONO_TABLE_DOCUMENT, /* 0x30 */
		MONO_TABLE_METHODBODY,
		MONO_TABLE_LOCALSCOPE,
		MONO_TABLE_LOCALVARIABLE,
		MONO_TABLE_LOCALCONSTANT,
		MONO_TABLE_IMPORTSCOPE,
		MONO_TABLE_STATEMACHINEMETHOD,
		MONO_TABLE_CUSTOMDEBUGINFORMATION

	#define MONO_TABLE_LAST MONO_TABLE_CUSTOMDEBUGINFORMATION
	#define MONO_TABLE_NUM (MONO_TABLE_LAST + 1)

	} MonoMetaTableEnum;





	namespace mono_data
	{
		static std::unordered_map<std::string, uintptr_t> mono_cached;
		static uintptr_t mono_dll;

		static bool ensure_mono_dll()
		{
			if (!mono_dll)
			{
				if (mono_cached.size()) mono_cached.clear();
				mono_dll = util::get_process_module(c(MONO_DLL_MODULE));
			}
			return mono_dll != NULL;
		}

		template<typename mono_t>
		mono_t create_function(const std::string& name)
		{
			if (!ensure_mono_dll()) return (mono_t)NULL;

			decltype(mono_cached)::iterator it = mono_cached.find(name);
			if (it != mono_cached.end())
				return (mono_t)it->second;

			uintptr_t pfn = util::get_proc_address(mono_dll, name, true);
			if (!pfn)
				return (mono_t)NULL;

			mono_cached[name] = pfn;
			return (mono_t)pfn;
		}
	}



	typedef enum {
		MONO_CALL_DEFAULT,
		MONO_CALL_C,
		MONO_CALL_STDCALL,
		MONO_CALL_THISCALL,
		MONO_CALL_FASTCALL,
		MONO_CALL_VARARG = 0x05,
		/* unused, */
		/* unused, */
		/* unused, */
		MONO_CALL_UNMANAGED_MD = 0x09, /* default unmanaged calling convention, with additional attributed encoded in modopts */
	} MonoCallConvention;


#define MONO_ALL_ASSEMBLIES c("\\all_assemblies")

	class c_mono
	{
	public:
		c_mono() {}
		~c_mono() {}


	public:
		uintptr_t(*mono_get_root_domain)() = NULL;
		uintptr_t(*mono_thread_attach)(uintptr_t domain) = NULL;
		uintptr_t(*mono_thread_detach)(uintptr_t thread) = NULL;
		uintptr_t(*mono_domain_assembly_open)(uintptr_t domain, const char *name) = NULL;
		uintptr_t(*mono_assembly_get_image)(uintptr_t assembly) = NULL;
		int32_t(*mono_image_get_table_rows)(uintptr_t image, uint32_t table) = NULL;
		uintptr_t(*mono_class_get)(uintptr_t image, uint32_t index) = NULL;
		const char* (*mono_class_get_name)(uintptr_t klass) = NULL;
		uintptr_t(*mono_class_get_method_from_name)(uintptr_t klass, const char* method_name, int32_t arg_count) = NULL;
		uintptr_t(*mono_compile_method)(uintptr_t method) = NULL;
		uintptr_t(*mono_class_get_methods)(uintptr_t klass, uintptr_t* iter) = NULL;
		const char* (*mono_method_get_name)(uintptr_t method) = NULL;
		uintptr_t(*mono_class_from_name)(uintptr_t image, const char* ns_name, const char* klass_name) = NULL;
		uintptr_t(*mono_class_get_fields)(uintptr_t klass, uintptr_t* iter) = NULL;
		const char* (*mono_field_get_name)(uintptr_t field) = NULL;
		uintptr_t(*mono_field_get_offset)(uintptr_t field) = NULL;
		uintptr_t(*mono_class_get_type)(uintptr_t klass) = NULL;
		void(*mono_assembly_foreach)(void(*func)(uintptr_t callback, uintptr_t ud), uintptr_t ud) = NULL;
		uintptr_t(*mono_method_signature)(uintptr_t method) = NULL;
		const char* (*mono_class_get_namespace)(uintptr_t klass) = NULL;
		//uintptr_t(*mono_method_get_unmanaged_thunk)(uintptr_t method) = NULL;
		//uintptr_t(*mono_runtime_invoke)(uintptr_t method, void* obj, uintptr_t* params, uintptr_t* exc) = NULL;
		uintptr_t(*mono_type_get_object)(uintptr_t domain, uintptr_t type);

		//uintptr_t(*mono_object_unbox)(uintptr_t object) = NULL;

		uintptr_t(*mono_string_new_len)(uintptr_t domain, const char* text, uint32_t len);


		uintptr_t mono_default_assembly = NULL;
		std::unordered_map<uint32_t, uintptr_t> mono_threads = std::unordered_map<uint32_t, uintptr_t>();



	public: // global mono functions
		static c_mono* singleton();
		static void cleanup();


	public: // public mono helpers
		// these should help make life easier
		inline uintptr_t default_assembly(const std::string& assembly_name = "")
		{
			if (mono_default_assembly)
				return mono_default_assembly;
			if (assembly_name.empty())
				return mono_default_assembly = open_assembly(c(ASSUMED_DEFAULT_MONO_ASSEMBLY));
			return mono_default_assembly = open_assembly(assembly_name);
		}

		inline uint16_t get_str_id(const char* val)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			std::u16string dest = convert.from_bytes(val);
			return *(uint16_t *)&dest[0];
		}

		inline std::string name_reality_check(const char* tname)
		{
			std::string name = tname;
			if (name.size() > 2)
			{
				for (int64_t i = 0; i < name.size() - 2; i++)
				{
					if (((uint8_t)name[i]) == 0xEE)
					{
						char buff[16];
						memset(buff, 0, sizeof(buff));
						sprintf_s(buff, sizeof(buff), "\\u%04X", get_str_id(tname + i));
						return name.substr(0, i) + std::string(buff);
					}
				}
			}
			return name;
		}


	public: // mono wrapper functions
		inline uintptr_t open_assembly(const std::string& assembly_name) { return mono_domain_assembly_open(mono_get_root_domain(), assembly_name.c_str()); }
		inline void open_all_assemblies(std::list<uintptr_t>& assembly_list)
		{
			mono_assembly_foreach([](uintptr_t assembly, uintptr_t ud)
			{
				std::list<uintptr_t>* m = (std::list<uintptr_t>*)ud;
				m->push_back(assembly);
			}, (uintptr_t)&assembly_list);
		}


	public: // mono interaction functions
		uintptr_t get_obfuscated_class(const std::string& klass_name, const std::string& assembly_name = "");
		uintptr_t find_class(const std::string& klass_name, const std::string& assembly_name = "", const std::string& ns = "");
		uintptr_t get_class_filtered(const std::string& name_space, const std::string& klass_name, const std::string& assembly_name = "");
		uintptr_t get_method_from_class(uintptr_t klass, const std::string& method_name, int arg_count = -1);
		//uintptr_t get_method_from_class_raw(uintptr_t klass, const std::string& method_name, int arg_count = -1);
		uintptr_t get_method(const std::string& name_string, const std::string& assembly_name = "");
		//uintptr_t get_method_from_class_unmanaged(uintptr_t klass, const std::string& method_name, int arg_count = -1);
		uintptr_t typeof(uintptr_t klass);

		uint32_t get_class_member_offset(uintptr_t klass, const std::string& member_name);


		MonoCallConvention get_method_convention(uintptr_t klass, const std::string& method_name);


		void attach();
		void detach();
	};
}