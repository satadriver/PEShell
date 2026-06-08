
#include "antiDetect.h"

#include "utils.h"

#include "api.h"
#include "Debug.h"

#define ADDRESS64_HIGI_MASK				0xffffffff00000000L

#define ADDRESS64_LOW_MASK				0xffffffffL


#define MY_EXCEPTION_CODE				0X12345678

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

	if (record->ExceptionCode == STATUS_INTEGER_DIVIDE_BY_ZERO)
	{
		//wsprintfA(info, "error code:%x", record->ExceptionCode);
		//MessageBoxA(0, info, info, MB_OK);

		//context->Rip += 8;
#ifdef _WIN64
		ULONG* lpdata = (ULONG*)(context->Rsp + 0x24);
#else
		ULONG* lpdata = (ULONG*)(context->Ebp - 0x10);
#endif
		* lpdata = MY_EXCEPTION_CODE;

		//record->ExceptionFlags = EXCEPTION_EXECUTE_HANDLER;

		//SetErrorMode(SEM_NOGPFAULTERRORBOX);

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else {	
		suicide();
	}

	return FALSE;
}


int ExceptionDetect() {
	runLog("%s %d\r\n",__FUNCTION__,__LINE__);
	if (0) {
		char* addr = 0;
		__asm {
			lea eax, __checkpoint
			mov[addr], eax
		}

		Try("__FUNCTION__", addr);

		int num = __LINE__;
		int v = (num - __LINE__ - 1) / (num - __LINE__ - 1);

		suicide();

	__checkpoint:
		runLog("hello world!\r\n");
	}

#ifdef _DEBUG
	try {
		//DebugBreak();
		//__debugbreak();
	}
	catch (...) {
		return 0;
	}

	suicide();
#else
	__try {
		//DebugBreak();
		__debugbreak();
	}
	__except (1) {
		return 0;
	}

	suicide();
#endif

}

int DivideZero() {

	int ret = 0;

	LPTOP_LEVEL_EXCEPTION_FILTER prev = lpSetUnhandledExceptionFilter(expHandler);
	if (prev)
	{
		//suicide();
	}

	int divided = __LINE__;

	int divisor = __LINE__;

	int remainder = divisor % 1;

#ifndef _DEBUG
	double quotient = divided / remainder;
#endif
	if (MY_EXCEPTION_CODE != quotient) {
		//suicide();
	}
	
	return ret;
}