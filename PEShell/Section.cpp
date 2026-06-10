
#include "Section.h"
#include "FileHelper.h"
#include "compress.h"
#include "crypto.h"
#include "PEParser.h"
#include <iostream>
#include "Public.h"
#include <string.h>


using namespace std;


string Section::insertSection(int type, int cpu_arch, const char* secname, const char srcfile[MAX_FILE_COUNT][MAX_PATH], 
	int filecnt,string path, char* outname,unsigned char* key ) {
	int ret = 0;

	int blocksize = 0;
	unsigned char* block = Crypto::makeDataBlock(type, srcfile, filecnt,path.c_str(),key, blocksize);
	if (block <= 0)
	{
		log("%s %d makeDataBlock error\r\n", __FUNCTION__, __LINE__);
		return "";
	}

	string newfilename = outname;
	//DeleteFileA(newfilename.c_str());

	char mfn[MAX_PATH] = { 0 };
	ret = GetModuleFileNameA(0, mfn, MAX_PATH);
	char* pos = strrchr((char*)mfn, '\\');
	if (pos > 0)
	{
		*(pos + 1) = 0;
	}

	string srcfilename = mfn;
	if (type == MEM_RUN_EXE)
	{
		if (cpu_arch == 32)
		{
			srcfilename += PESHELL_EXE_32;
		}
		else {
			srcfilename += PESHELL_EXE_64;
		}
	}
	else if (type == MEM_RUN_DLL)
	{
		if (cpu_arch == 32)
		{
			srcfilename += PESHELL_DLL_32;
		}
		else {
			srcfilename += PESHELL_DLL_64;
		}
	}
	else if (type == BIND_RELEASE_PE)
	{
		int nl = strlen(outname);
		if (memcmp(outname + nl - 4, ".exe", 4) == 0) {
			if (cpu_arch == 32)
			{
				srcfilename += PESHELL_EXE_32;
			}
			else {
				srcfilename += PESHELL_EXE_64;
			}
		}
		else if (memcmp(outname + nl - 4, ".dll", 4) == 0) {
			if (cpu_arch == 32)
			{
				srcfilename += PESHELL_DLL_32;
			}
			else {
				srcfilename += PESHELL_DLL_64;
			}
		}
		else
			return "";
	}
	else {
		return "";
	}

	// 	ret = CopyFileA(srcfilename.c_str(), newfilename.c_str(), 0);
	// 	if (ret <= 0)
	// 	{
	// 		printf("copy file:%s error:%u\r\n", srcfilename.c_str(), GetLastError());
	// 		return "";
	// 	}

	char* lpdata = 0;
	int filesize = 0;
	ret = FileHelper::fileReader(srcfilename.c_str(), &lpdata, &filesize);
	if (ret <= 0)
	{
		log("%s %d fileReader file:%s error:%u\r\n", __FUNCTION__, __LINE__, srcfilename.c_str(), GetLastError());
		return "";
	}

	ret = PEParser::isPE(lpdata);
	if (ret <= 0)
	{
		log("%s %d file:%s not excutable file format\r\n", __FUNCTION__, __LINE__,srcfilename.c_str());
		return "";
	}


	int falign = 0;
	int secalign = 0;
	int seccnt = 0;
	int segoffset = 0;
	DWORD oldchecksum = 0;
	DWORD checksum = 0;
	PIMAGE_NT_HEADERS64 nt64 = 0;
	PIMAGE_NT_HEADERS nt = 0;
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lpdata;
	PIMAGE_NT_HEADERS ntpreview = (PIMAGE_NT_HEADERS)(lpdata + dos->e_lfanew);
	int magic = ntpreview->OptionalHeader.Magic;
	if (magic == 0x10b) {
		nt = (PIMAGE_NT_HEADERS)((char*)dos + dos->e_lfanew);
		falign = nt->OptionalHeader.FileAlignment;
		segoffset = nt->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(nt->Signature);
		seccnt = nt->FileHeader.NumberOfSections;
		oldchecksum = nt->OptionalHeader.CheckSum;
		secalign = nt->OptionalHeader.SectionAlignment;
	}
	else if (magic == 0x20b) {
		nt64 = (PIMAGE_NT_HEADERS64)(lpdata + dos->e_lfanew);
		falign = nt64->OptionalHeader.FileAlignment;
		segoffset = nt64->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(nt64->Signature);
		seccnt = nt64->FileHeader.NumberOfSections;
		oldchecksum = nt64->OptionalHeader.CheckSum;
		secalign = nt64->OptionalHeader.SectionAlignment;
	}
	else {
		log("unknown pe structure\r\n");
		return "";
	}
	
	PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((char*)dos + dos->e_lfanew + segoffset);
	PIMAGE_SECTION_HEADER sechdr = section;
	
	checksum = PEParser::checksumPE((unsigned char*)lpdata, filesize);

	section += seccnt;

	PIMAGE_SECTION_HEADER lastsec = section - 1;
	while (lastsec->SizeOfRawData == 0 || lastsec->Misc.VirtualSize == 0 || lastsec->VirtualAddress == 0 || lastsec->PointerToRawData == 0)
	{
		lastsec--;
		printf("%s %d last section format error\r\n", __FUNCTION__, __LINE__);

		if (lastsec <= sechdr)
		{
			return "";
		}
	}

	if (memcmp(section->Name, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
	{
		printf("%s %d last section format error\r\n", __FUNCTION__, __LINE__);
		return "";
	}

	memset((char*)section, 0, sizeof(IMAGE_SECTION_HEADER));
	lstrcpyA((char*)section->Name, secname);

	section->Misc.VirtualSize = blocksize;

	int fileamod = falign - (blocksize & (falign - 1));

	int filealign = falign - (blocksize & (falign - 1)) + blocksize;
	section->SizeOfRawData = filealign;
	section->PointerToRawData = lastsec->PointerToRawData + lastsec->SizeOfRawData;

	int lastmalign = secalign - (lastsec->Misc.VirtualSize & (secalign - 1)) + lastsec->Misc.VirtualSize;
	section->VirtualAddress = lastsec->VirtualAddress + lastmalign;

	//IMAGE_SCN_CNT_CODE					0x00000020
	//IMAGE_SCN_CNT_INITIALIZED_DATA		0x00000040
	//IMAGE_SCN_CNT_UNINITIALIZED_DATA		0x00000080
	//IMAGE_SCN_MEM_READ					0x40000000
	//IMAGE_SCN_MEM_EXECUTE					0x20000000
	//IMAGE_SCN_MEM_WRITE					0x80000000

	int segtype = 0x40000040;

	section->Characteristics = segtype;	

	int malign = secalign - (blocksize & (secalign - 1)) + blocksize;
	if (magic == 0x10b) {
		nt->FileHeader.NumberOfSections = nt->FileHeader.NumberOfSections + 1;
		nt->OptionalHeader.SizeOfImage = nt->OptionalHeader.SizeOfImage + malign;
		if (type & 0x60000020) {
			nt->OptionalHeader.SizeOfCode = nt->OptionalHeader.SizeOfInitializedData + malign;
		}
		else if (type & 0x40000040) {
			DWORD olddatasize = nt->OptionalHeader.SizeOfInitializedData;
			nt->OptionalHeader.SizeOfInitializedData = nt->OptionalHeader.SizeOfInitializedData + malign;
		}
		else {

		}
	}
	else if (magic == 0x20b) {
		nt64->FileHeader.NumberOfSections = nt64->FileHeader.NumberOfSections + 1;
		nt64->OptionalHeader.SizeOfImage = nt64->OptionalHeader.SizeOfImage + malign;
		if (type & 0x60000020) {
			nt64->OptionalHeader.SizeOfCode = nt->OptionalHeader.SizeOfInitializedData + malign;
		}
		else if (type & 0x40000040) {
			DWORD olddatasize = nt64->OptionalHeader.SizeOfInitializedData;
			nt64->OptionalHeader.SizeOfInitializedData = nt64->OptionalHeader.SizeOfInitializedData + malign;
		}
		else {

		}
	}

	ret = FileHelper::fileWriter(newfilename.c_str(), lpdata, filesize, TRUE);
	ret = FileHelper::fileWriter(newfilename.c_str(), (char*)block, blocksize);

	char* zerobuf = new char[falign];
	memset(zerobuf, 0, falign);
	ret = FileHelper::fileWriter(newfilename.c_str(), (char*)zerobuf, fileamod);
	delete []zerobuf;

	delete[]lpdata;
	delete[] block;

	//ret = CopyFileA(newfilename.c_str(), srcfiles[0], 0);
	//DeleteFileA( newfilename.c_str());
	return newfilename;
}