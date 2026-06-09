
#include "antiDetect.h"

#include "utils.h"

#include "api.h"
#include "Debug.h"

#define ADDRESS64_HIGI_MASK				0xffffffff00000000L

#define ADDRESS64_LOW_MASK				0xffffffffL


#define MY_EXCEPTION_CODE				(0X12345678)

 extern "C" void asmJunkCode();

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

int g_opcode_len = 2;
char* g_exp_pc = 0;

LONG __stdcall expHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
	PEXCEPTION_RECORD record = ExceptionInfo->ExceptionRecord;

	PCONTEXT context = ExceptionInfo->ContextRecord;

	char info[1024];

	runLog("exception code:%x,address:%x\r\n", record->ExceptionCode, record->ExceptionAddress);

	if (record->ExceptionCode == STATUS_INTEGER_DIVIDE_BY_ZERO)
	{
		
#ifdef _WIN64
		unsigned char opcode = *(unsigned char*)context->Rip;
		if (opcode == 0xf7 || opcode == 0xf6) {
			if (g_exp_pc == 0)
				g_exp_pc = (char*)context->Rip;
			context->Rip = (DWORD64)g_exp_pc + g_opcode_len;
		}
		
		ULONG* lpdata = (ULONG*)(context->Rsp + 0x24);		
		//*lpdata = MY_EXCEPTION_CODE;
#else
		ULONG* lpdata = (ULONG*)(context->Ebp - 0x10);
		//*lpdata = MY_EXCEPTION_CODE;
		unsigned char opcode = *(unsigned char*)context->Eip;
		if (opcode == 0xf7 || opcode == 0xf6) 
		{
			if(g_exp_pc == 0)
				g_exp_pc = (char*) context->Eip;
			context->Eip = (DWORD)g_exp_pc + g_opcode_len;
		}
#endif
		//record->ExceptionFlags = EXCEPTION_EXECUTE_HANDLER;

		//SetErrorMode(SEM_NOGPFAULTERRORBOX);

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (record->ExceptionCode == STATUS_ILLEGAL_INSTRUCTION) {
#ifdef _WIN64
		unsigned char opcode = *(unsigned char*)context->Rip;
		context->Rip++;
#else
		unsigned char opcode = *(unsigned char*)context->Eip;
		context->Eip++;
#endif
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else {
		suicide();
	}

	return FALSE;
}


int ExceptionDetect() {
	runLog("%s %d\r\n",__FUNCTION__,__LINE__);

#ifndef _WIN64
	if (0) {
		char* addr = 0;
		__asm {
			lea eax, __checkpoint
			mov[addr], eax
		}

		Try(__FUNCTION__, (char*)addr);

		int num = __LINE__;
		int v = (num + 1 - __LINE__ ) / (num + 1 - __LINE__ );
		char* data = 0;
		//int v = *(DWORD*)data;

		suicide();

	__checkpoint:
		runLog("hello world!\r\n");
	}
#endif

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

	int divided = MY_EXCEPTION_CODE;

	//int divisor = __LINE__;

	int remainder = divided % 1;

#ifndef _DEBUG
	int quotient = divided / remainder;
#endif
	if ( (MY_EXCEPTION_CODE/ MY_EXCEPTION_CODE) != quotient) {
		runLog("%s %d quotient:%x,expectation:%x\r\n", __FUNCTION__, __LINE__, quotient,1);
		//suicide();
	}
	
	return ret;
}

#ifndef _WIN64
void asmJunkCode() {
	__asm {
		jz     _label_0; 条件永远为假，不会跳转
		jnz    _label_0; 条件永远为真，一定会跳转
		_emit      0xE8; 垃圾字节('call' 的操作码)
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0

		_label_0:
		jnz _label_1
		jz _label_1
		_emit      0xe9
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
		_label_1:
		jnz _label_2
		jz _label_2
		_emit      0xeb
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
		_label_2:
		jnz _label_3
		jz _label_3
		_emit      0x9a
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
			_emit      0
		_label_3:
		jnz _label_exit
		jz _label_exit
		_emit      0xea
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0
		_emit      0

		jmp _label_0

		_label_exit:
	}
}




#endif


void JunkCode() {
	asmJunkCode();
}