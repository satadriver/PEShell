#pragma once
#include <windows.h>

class PEParser {
public:
	static int isPE(const char * data);

	static int getMachine(const char * pedata);
	static int getNumberOfSections(const char * pedata);
	static int getTimeDateStamp(const char * pedata);
	static int getPointerToSymbolTable(const char * pedata);
	static int getNumberOfSymbols(const char * pedata);
	static int getSizeOfOptionalHeader(const char * pedata);
	static int getCharacteristics(const char * pedata);


	static PIMAGE_OPTIONAL_HEADER getOptionalHeader(const char * pedata);
	static PIMAGE_FILE_HEADER getNtHeader(const char * pedata);

	static int PEParser::getBaseApi(HMODULE * kernel32, DWORD * getprocaddr, DWORD * loadlib);
	static HMODULE PEParser::GetDllKernel32Base();

	static DWORD PEParser::checksumPE(unsigned char * data, int datasize);

	static FARPROC getProcAddr(HMODULE pmodule, const char * funname, int funnamelen);
};



//dll =210e exe =010f
/*
#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.

#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved externel references).

#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.

#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.

#define IMAGE_FILE_AGGRESIVE_WS_TRIM         0x0010  // Agressively trim working set

#define IMAGE_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses

#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.

#define IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.

#define IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file

#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.

#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.

#define IMAGE_FILE_SYSTEM                    0x1000  // System File.

#define IMAGE_FILE_DLL                       0x2000  // File is a DLL.

#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine

#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.
*/

//DllCharacteristics; // DllMain()函数何时被调用，默认为 0
//MajorOperatingSystemVersion; // 要求操作系统最低版本号的主版本号
//MajorImageVersion; // 可运行于操作系统的主版本号
//SizeOfImage 内存中整个PE映像体的尺寸。它是所有头和节经过节对齐处理后的大小。

/*
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem.

#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem.
*/

/*
#define IMAGE_NT_OPTIONAL_HDR32_MAGIC      0x10b  // 32位PE可选头

#define IMAGE_NT_OPTIONAL_HDR64_MAGIC      0x20b  // 64位PE可选头

#define IMAGE_ROM_OPTIONAL_HDR_MAGIC       0x107
*/

/*
#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory

#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory

#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory

#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory

#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory

#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table

#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory

//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)

#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data

#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP

#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory

#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory

#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers

#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table

#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors

#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor
*/