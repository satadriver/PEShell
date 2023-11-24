#include <stdio.h>
#include <windows.h>


extern "C" __declspec(dllexport) int test() {
	MessageBoxA(0, "test", "test", MB_OK);
	return 0;
}

extern "C" __declspec(dllexport) int mytest() {
	MessageBoxA(0, "mytest", "mytest", MB_OK);
	return 0;
}

extern "C" __declspec(dllexport) signed int SbieApi_Log()
{
	MessageBoxA(0,"SbieApi_Log","SbieApi_Log",0);
	return TRUE;
}

extern "C" __declspec(dllexport) void __stdcall  SbieDll_Hook(int a1, int a2, int a3)
{
	MessageBoxA(0,"SbieDll_Hook","SbieDll_Hook",0);
	return;
}

int __stdcall DllMain( _In_ HINSTANCE hInstance, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved ){
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		MessageBoxA(0, "start", "start", MB_OK);
	}else if (fdwReason == DLL_PROCESS_DETACH)
	{
		MessageBoxA(0, "end", "end", MB_OK);
	}

	return TRUE;
}

int main() {
	return 0;
}


int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){
	return 0;
}