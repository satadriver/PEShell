
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

	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)lpdata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((char*)dos + dos->e_lfanew);
	int segoffset = nt->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(nt->Signature);
	PIMAGE_SECTION_HEADER sections = (PIMAGE_SECTION_HEADER)((char*)dos + dos->e_lfanew + segoffset);
	PIMAGE_SECTION_HEADER sechdr = sections;
	int secscnt = nt->FileHeader.NumberOfSections;

	DWORD oldchecksum = nt->OptionalHeader.CheckSum;

	DWORD checksum = PEParser::checksumPE((unsigned char*)lpdata, filesize);

	sections += secscnt;

	PIMAGE_SECTION_HEADER lastsec = sections - 1;
	while (lastsec->SizeOfRawData == 0 || lastsec->Misc.VirtualSize == 0 || lastsec->VirtualAddress == 0 || lastsec->PointerToRawData == 0)
	{
		lastsec--;
		printf("%s %d last section format error\r\n", __FUNCTION__, __LINE__);

		if (lastsec <= sechdr)
		{
			return "";
		}
	}

	if (memcmp(sections->Name, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
	{
		printf("%s %d last section format error\r\n", __FUNCTION__, __LINE__);
		return "";
	}

	memset((char*)sections, 0, sizeof(IMAGE_SECTION_HEADER));
	lstrcpyA((char*)sections->Name, secname);

	//all pe section is from 0 to memmory high
	sections->Misc.VirtualSize = blocksize;

	int pefalignsize = nt->OptionalHeader.FileAlignment;
	int filemodadd = pefalignsize - (blocksize & (pefalignsize - 1));
	int filealignsize = filemodadd + blocksize;
	sections->SizeOfRawData = filealignsize;
	sections->PointerToRawData = lastsec->PointerToRawData + lastsec->SizeOfRawData;

	int secalign = nt->OptionalHeader.SectionAlignment;
	int lastmemalignsize = secalign - (lastsec->Misc.VirtualSize & (secalign - 1)) +
		lastsec->Misc.VirtualSize;
	sections->VirtualAddress = lastsec->VirtualAddress + lastmemalignsize;

	sections->Characteristics = 0x40000040;	//rdata
	//sections->Characteristics = 0x40000000;

	nt->FileHeader.NumberOfSections = nt->FileHeader.NumberOfSections + 1;

	int memalignsize = secalign - (blocksize & (secalign - 1)) + blocksize;
	nt->OptionalHeader.SizeOfImage = nt->OptionalHeader.SizeOfImage + memalignsize;

	DWORD olddatasize = nt->OptionalHeader.SizeOfInitializedData;
	nt->OptionalHeader.SizeOfInitializedData = nt->OptionalHeader.SizeOfInitializedData + memalignsize;		//memalignsize?

	ret = FileHelper::fileWriter(newfilename.c_str(), lpdata, filesize, TRUE);
	ret = FileHelper::fileWriter(newfilename.c_str(), (char*)block, blocksize);
	char* zerobuf = new char[pefalignsize];
	memset(zerobuf, 0, pefalignsize);
	ret = FileHelper::fileWriter(newfilename.c_str(), (char*)zerobuf, filemodadd);
	delete []zerobuf;

	delete[]lpdata;
	delete[] block;

	//ret = CopyFileA(newfilename.c_str(), srcfiles[0], 0);
	//DeleteFileA( newfilename.c_str());
	return newfilename;
}