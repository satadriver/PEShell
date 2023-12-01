
#include "PEParser.h"
#include <windows.h>
#include "Public.h"
#include "api.h"


int PEParser::isPE(const char * data) {
	char szpedoshdr[] = { 'M','Z' };
	if (memcmp(data, szpedoshdr,2) == 0)
	{
		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)data;
		PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(data + dos->e_lfanew);
		unsigned char szpehdr[] = { 'P','E',0,0 };
		if (memcmp((char*)&nt->Signature, szpehdr,4) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}



int PEParser::getMachine(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->Machine;
}

int PEParser::getNumberOfSections(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->NumberOfSections;
}

int PEParser::getTimeDateStamp(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->TimeDateStamp;
}

int PEParser::getPointerToSymbolTable(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->PointerToSymbolTable;
}

int PEParser::getNumberOfSymbols(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->NumberOfSymbols;
}

int PEParser::getSizeOfOptionalHeader(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->SizeOfOptionalHeader;
}


int PEParser::getCharacteristics(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh->Characteristics;
}



PIMAGE_OPTIONAL_HEADER PEParser::getOptionalHeader(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_OPTIONAL_HEADER opt = &nt->OptionalHeader;
	return opt;
}
PIMAGE_FILE_HEADER PEParser::getNtHeader(const char * pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	PIMAGE_FILE_HEADER fh = &nt->FileHeader;
	return fh;
}



FARPROC PEParser::getProcAddr(HMODULE pmodule, const char * funname, int funnamelen)
{
#ifndef _WIN64
	FARPROC pGetProcAddr = 0;
	__asm
	{
		pushad

		mov edi, pmodule

		mov eax, [edi + 3ch]

		mov edx, [edi + eax + 78h]	//import table base

		add edx, edi			//import table

		mov ecx, [edx + 18h]; number of functions

		mov ebx, [edx + 20h]

		add ebx, edi; AddressOfName

		__search :

		dec ecx
			cmp ecx, 0xffffffff
			jz __funcend

			mov esi, [ebx + ecx * 4]

			add esi, edi

			push edi
			push esi
			push ecx
			mov ecx, funnamelen
			mov edi, funname
			cld

			repz cmpsb
			jnz _tonotfound

			jecxz __foundname

			_tonotfound:
			pop ecx
			pop esi
			pop edi
			jmp __search



			__foundname :
			pop ecx
			pop esi
			pop edi

		mov ebx, [edx + 24h]

			add ebx, edi; index address

			movzx ecx, word ptr[ebx + ecx * 2]

			mov ebx, [edx + 1ch]

			add ebx, edi

			mov eax, [ebx + ecx * 4]

			add eax, edi

			mov pGetProcAddr, eax

			__funcend :
		popad
	}

	return  pGetProcAddr;

#else
	return (FARPROC)GetProcAddress(pmodule, funname);
#endif
}

HMODULE PEParser::GetDllKernel32Base()
{
#ifndef _WIN64
	HMODULE hKernel32 = 0;
	__asm
	{
		//assume fs:nothing
		push esi
		xor eax, eax
		mov eax, fs:[eax + 30h]		; 指向PEB的指针
		mov eax, [eax + 0ch]		; 指向PEB_LDR_DATA的指针
		mov esi, [eax + 0ch]		; 根据PEB_LDR_DATA得出InLoadOrderModuleList的Flink字段
		lodsd
		mov eax, [eax]				; 指向下一个节点
		mov eax, [eax + 18h]		; Kernel.dll的基地址
		mov hKernel32, eax
		pop esi
	}

	return hKernel32;
#else
	char szKernel32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	return GetModuleHandleA(szKernel32);
#endif
}

int PEParser::getBaseApi(HMODULE * kernel32,FARPROC * getprocaddr, FARPROC* loadlib) {
#ifndef _WIN32
	char szLoadLibraryA[] = { 'L','o','a','d','L','i','b','r','a','r','y','A',0 };
	char szGetProcAddress[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s',0 };

	*kernel32 = (HMODULE)GetDllKernel32Base();
	if (kernel32 == 0)
	{
		Public::writelog("kernel32 null\r\n");
		return FALSE;
	}

	*getprocaddr = (FARPROC)getProcAddr(*kernel32, szGetProcAddress, lstrlenA(szGetProcAddress));
	*loadlib = (FARPROC)getProcAddr(*kernel32, szLoadLibraryA, lstrlenA(szLoadLibraryA));
#else
	char szKernel32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	*kernel32=GetModuleHandleA(szKernel32);
	* getprocaddr = (FARPROC) GetProcAddress;
	*loadlib = (FARPROC)LoadLibraryA;
#endif

	return TRUE;
}

DWORD PEParser::checksumPE(unsigned char * data, int datasize) {
	DWORD checksum = 0;
#ifndef _WIN64
	__asm {
		pushad
		xor eax, eax
		mov ebx, datasize
		mov ecx, ebx
		push ecx
		shr ecx, 1
		mov esi, data
		clc
		__cal_checksum :
		adc ax, word ptr[esi]
			inc esi
			inc esi
			loop __cal_checksum
			adc ax, 0

			pop ecx
			test ecx, 1
			jz __end
			xor edi, edi
			movzx di, byte ptr[esi]
			clc
			add ax, di
			__end :
		add eax, ebx;
		mov checksum, eax
		popad
	}
#endif
	return checksum;
}
