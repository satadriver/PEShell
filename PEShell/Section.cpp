
#include "Section.h"
#include "FileHelper.h"
#include "compress.h"
#include "crypto.h"
#include "PEParser.h"
#include <iostream>


using namespace std;


string Section::insertSection(int type, int cpu_arch, const char* secname, const char srcfiles[MAX_FILE_COUNT][MAX_PATH], 
	int srcfilescnt,string path, char* outname) {
	int ret = 0;

	int blocksize = 0;
	unsigned char* block = Crypto::makeDataBlock(type, srcfiles, srcfilescnt,path.c_str(), blocksize);
	if (block <= 0)
	{
		printf("makeDataBlock error\r\n");
		return "";
	}

	string newfilename = outname;
	//DeleteFileA(newfilename.c_str());

	char szcurdir[MAX_PATH] = { 0 };
	ret = GetModuleFileNameA(0, szcurdir, MAX_PATH);
	char* pos = strrchr((char*)szcurdir, '\\');
	if (pos > 0)
	{
		*(pos + 1) = 0;
	}

	string srcfilename = szcurdir;
	if (type == 1)
	{
		if (cpu_arch == 0x014c)
		{
			srcfilename += "PeUnshell.exe";
		}
		else {
			srcfilename += "PeUnshell.exe";
		}
	}
	else if (type == 2)
	{
		//srcfilename += "PeUnshell.dll";
		if (cpu_arch == 0x014c)
		{
			srcfilename += "PeUnshell.dll";
		}
		else {
			srcfilename += "PeUnshell.dll";
		}
	}
	else if (type == 3)
	{
		//srcfilename += "PeUnshell.exe";
		if (cpu_arch == 0x014c)
		{
			srcfilename += "PeUnshell.exe";
		}
		else {
			srcfilename += "PeUnshell.exe";
		}
	}
	else if (type == 4)
	{
		//srcfilename += "PeUnshell.dll";
		if (cpu_arch == 0x014c)
		{
			srcfilename += "PeUnshell.dll";
		}
		else {
			srcfilename += "PeUnshell.dll";
		}
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
		printf("fileReader file:%s error:%u\r\n", srcfilename.c_str(), GetLastError());
		return "";
	}

	ret = PEParser::isPE(lpdata);
	if (ret <= 0)
	{
		printf("file:%s is not pe format\r\n", srcfilename.c_str());
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
		printf("the last section is not correct,please check it\r\n");

		if (lastsec <= sechdr)
		{
			return "";
		}
	}

	if (memcmp(sections->Name, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
	{
		printf("the last section is not correct,please check it\r\n");
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