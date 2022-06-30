#include "funcs.h"
#include <util/structs/ldr.h>



// Absolutely did *NOT* copy this from my driver. At all. Promise. :)
uintptr_t util::get_process_module(const std::wstring& module_name)
{
	PEB *pPeb = (PEB *)__readgsqword(0x60);
	if (!pPeb)
		return 0;

	PEB_LDR_DATA *pLdr = (PEB_LDR_DATA *)pPeb->Ldr;
	for (LIST_ENTRY *pEntry = (LIST_ENTRY *)pLdr->InMemoryOrderModuleList.Flink; pEntry != &pLdr->InMemoryOrderModuleList; pEntry = (LIST_ENTRY *)pEntry->Flink)
	{
		LDR_DATA_TABLE_ENTRY *entry = CONTAINING_RECORD(pEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		if (module_name.empty())
			return (uintptr_t)entry->DllBase;
		else if (!_memicmp(entry->BaseDllName.Buffer, module_name.c_str(), module_name.size() > entry->BaseDllName.Length ? entry->BaseDllName.Length : module_name.size()))
				return (uintptr_t)entry->DllBase;
	}
	return 0;
}


uintptr_t util::get_proc_address(const uintptr_t module_base, const std::string& proc_name, bool case_insensitive)
{
	int(* const f_strcmp)(const char *s1, const char *s2) = case_insensitive ? _strcmpi : strcmp;
	
	PIMAGE_DOS_HEADER dos_header = (IMAGE_DOS_HEADER *)module_base;
	if (!dos_header) return 0; // how?

	PIMAGE_NT_HEADERS64 nt_header = (PIMAGE_NT_HEADERS64)(module_base + dos_header->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY export_dir = (PIMAGE_EXPORT_DIRECTORY)(module_base + nt_header->OptionalHeader.DataDirectory[0].VirtualAddress);

	uint32_t *rva_names = (uint32_t *)(module_base + export_dir->AddressOfNames);
	uint16_t *name_ordinals = ((uint16_t *)(module_base + export_dir->AddressOfNameOrdinals));
	uint32_t *function_offsets = ((uint32_t *)(module_base + export_dir->AddressOfFunctions));

	for (uint32_t i = 0; i < export_dir->NumberOfNames; i++)
	{
		if (!f_strcmp((char *)(module_base + rva_names[i]), proc_name.c_str()))
			return module_base + function_offsets[name_ordinals[i]];
	}
	return NULL;
}