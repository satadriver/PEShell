
#include "antiDetect.h"

#include "utils.h"

#include "api.h"


#define ADDRESS64_HIGI_MASK				0xffffffff00000000L

#define ADDRESS64_LOW_MASK				0xffffffffL

AntiDetect::AntiDetect() {

}
AntiDetect::~AntiDetect() {

}

int  AntiDetect::checkHook() {

	int res = FALSE;
	char szkernel32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	FARPROC addr = GetProcAddress(lpLoadLibraryA(szkernel32), "CreateFile");

	unsigned char* p = (unsigned char*)addr;
#ifdef _WIN64

	if (p[0] == 0xff && p[1] == 0x25)
	{
		DWORD offset = *(DWORD*)(p + 2);

		ULONGLONG* dst = (ULONGLONG*)(p + 6 + offset);

		FARPROC f = (FARPROC)*dst;

	}
	else if (p[0] == 0x48 && p[1] == 0xff && p[2] == 0x25)
	{
		DWORD offset = *(DWORD*)(p + 3);

		ULONGLONG * dst = (ULONGLONG*) (p + 7 + offset);

		FARPROC f = (FARPROC )*dst;

	}
#else
	if (p[0] == 0xe9 )
	{
		DWORD offset = *(DWORD*)(p + 1);
		ULONG* dst = (ULONG*)(p + 5 + offset);

		FARPROC f = (FARPROC)*dst;
	}
#endif

	return 0;
}



LONG __stdcall expHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
	PEXCEPTION_RECORD record = ExceptionInfo->ExceptionRecord;

	PCONTEXT context = ExceptionInfo->ContextRecord;

	char info[1024];

	runLog("exception code:%x,address:%x\r\n", record->ExceptionCode, record->ExceptionAddress);

	if (record->ExceptionCode == 0xC0000094)
	{
		//wsprintfA(info, "error code:%x", record->ExceptionCode);
		//MessageBoxA(0, info, info, MB_OK);

		//context->Rip += 8;
#ifdef _WIN64
		ULONG* lpdata = (ULONG*)(context->Rsp + 0x24);
#else
		ULONG* lpdata = (ULONG*)(context->Ebp - 0x10);
#endif
		* lpdata = 1;

		//record->ExceptionFlags = EXCEPTION_EXECUTE_HANDLER;

		//SetErrorMode(SEM_NOGPFAULTERRORBOX);

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else {	
		suicide();
	}

	return FALSE;
}



int exceptTest() {

	int ret = 0;

	LPTOP_LEVEL_EXCEPTION_FILTER prev = lpSetUnhandledExceptionFilter(expHandler);
	if (prev)
	{
		//suicide();
	}

	int divided = 0;

	int divisor = 2;

	int remainder = divisor % 1;

#ifndef _DEBUG
	double quotient = divided / remainder;
#endif

	return ret;
}