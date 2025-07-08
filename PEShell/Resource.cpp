
#include "Resource.h"
#include <stdio.h>


int ResourceParser(DWORD module,DWORD resbase, PIMAGE_RESOURCE_DIRECTORY dir,int level,DWORD id,DWORD & offset,DWORD &size) 
{
	int ret = 0;

	while (1)
	{
		if (dir->NumberOfIdEntries == 0 && dir->NumberOfNamedEntries == 0)
		{
			return 0;
		}

		int cnt = dir->NumberOfIdEntries + dir->NumberOfNamedEntries;
		PIMAGE_RESOURCE_DIRECTORY_ENTRY entry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)dir + sizeof(IMAGE_RESOURCE_DIRECTORY));
		for (int i = 0; i < cnt ; i ++)
		{
			DWORD addr = entry->OffsetToData;
			if (addr & 0x80000000)
			{
				DWORD name = entry->Name;
				if (name & 0x80000000)
				{
					name = name & 0x7fffffff;
					char buf[1024];
					PIMAGE_RESOURCE_DIR_STRING_U str = (PIMAGE_RESOURCE_DIR_STRING_U)(resbase+name);
					ret = WideCharToMultiByte(CP_ACP, 0, str->NameString, str->Length, buf, 1024, 0, 0);
					printf("get resource level:%u name:%s\r\n",level, buf);
				}
				else {
					printf("get resource level:%u type:%u:\r\n",level, name);
				}

				addr = addr & 0x7fffffff;
				PIMAGE_RESOURCE_DIRECTORY next = (PIMAGE_RESOURCE_DIRECTORY)(resbase + addr);
				ret = ResourceParser(module, resbase, next, level++,id,offset,size);
				entry++;
			}
			else {
				PIMAGE_RESOURCE_DATA_ENTRY data = (PIMAGE_RESOURCE_DATA_ENTRY)(resbase + addr);
				offset = data->OffsetToData + module;
				size = data->Size;

				DWORD name = entry->Name;
				if (name & 0x80000000)
				{
					name = name & 0x7fffffff;
					char buf[1024];
					PIMAGE_RESOURCE_DIR_STRING_U str = (PIMAGE_RESOURCE_DIR_STRING_U)(name + resbase);
					ret = WideCharToMultiByte(CP_ACP, 0, str->NameString, str->Length, buf, 1024, 0, 0);
					printf("get item name:%s,size:%u,address:%x\r\n", buf,size,offset);
				}
				else {
					printf("get item id:%u,size:%u,address:%x\r\n", name,size,offset);
				}

				entry++;
			}
		}

		dir = (PIMAGE_RESOURCE_DIRECTORY)entry;
	}
}


int Resource::getResource(DWORD module,int id) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(module + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;

	PIMAGE_RESOURCE_DIRECTORY res = (PIMAGE_RESOURCE_DIRECTORY)(nt->OptionalHeader.DataDirectory[2].VirtualAddress + module);

	DWORD offset = 0;
	DWORD size = 0;
	ResourceParser(module,(DWORD)res, res, 1,id,offset,size);

	return 0;
}


int Resource::getResource(DWORD module, const char *name) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(module + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;

	//IMAGE_DIRECTORY_ENTRY_RESOURCE
	PIMAGE_RESOURCE_DIRECTORY res = (PIMAGE_RESOURCE_DIRECTORY)
		(nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress + module);

	DWORD offset = 0;
	DWORD size = 0;
	ResourceParser(module, (DWORD)res, res, 1,(DWORD)name,offset,size);

	return 0;
}