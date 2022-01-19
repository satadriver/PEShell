
#ifdef _WINDLL

#define DLLHIJACK_QQBROWSER
#define DLLHIJACK_LIBCURL
#define DLLHIJACK_VERSION
#define DLLHIJACK_SBIEDLL
#define DLLHIJACK_GETCURRENTROLLBACK
#define DLLHIJACK_GOOGLESERVICE
#define DLLHIJACK_QQMGRDIARY
#define DLLHIJACK_CLOVER
#define DLLHIJACK_MSOOBE
#define DLLHIJACK_REKEYWIZ
#define DLLHIJACK_91ASSISTUPDATE
#define DLLHIJACK_FIRSTLOAD


//dsktptst.dll
extern "C" __declspec(dllexport) void *__stdcall SetCommandLine(void *Src);

extern "C" __declspec(dllexport) int __stdcall ShowMessage(int a1);


//http://www.dependencywalker.com/ dependency下载地址
#ifdef DLLHIJACK_LIBCURL
extern "C" __declspec(dllexport) int __cdecl curl_easy_cleanup(int a1);
//用VS2005反汇编 固定参数时是callee管理的
//仅仅应用于"C++"成员函数。this指针存放于eCX寄存器，参数从右到左压
//__thiscall。类成员函数默认使用的调用约定。
//其规则是：参数从右向左压入堆栈，x86构架下this指针通过ECX寄存器传递，函数退出时由callee清理堆栈中的参数
//同样不支持可变数量的参数。如果显式地把类成员函数声明为使用__cdecl或者__stdcall，那么，
//将采用__cdecl或者__stdcall的规则来压栈和出栈，而this指针将作为函数的第一个参数最后压入堆栈，而不是使用ECX寄存器来传递了。
//extern "C" __declspec(dllexport) int __thiscall curl_easy_init(void *a1)
//extern "C" __declspec(dllexport) int __stdcall curl_easy_init(void *a1);
////实际测试为__cdecl调用，caller恢复堆栈
extern "C" __declspec(dllexport) int __cdecl curl_easy_init();
extern "C" __declspec(dllexport) int __cdecl curl_easy_perform(int a1);
extern "C" __declspec(dllexport) signed int __cdecl curl_easy_setopt(int a1, int a2, char a3);
#endif


#ifdef DLLHIJACK_SBIEDLL
extern "C" __declspec(dllexport) signed int SbieApi_Log();
extern "C" __declspec(dllexport) void __stdcall  SbieDll_Hook(int a1, int a2, int a3);
#endif


#ifdef DLLHIJACK_CLOVER
extern "C" __declspec(dllexport) DWORD /*__stdcall*/ CloverMain();
#endif

#ifdef DLLHIJACK_QQMGRDIARY
extern "C" __declspec(dllexport) DWORD __cdecl DestroyQMNetworkMgr();
extern "C" __declspec(dllexport) void __cdecl CreateQMNetworkMgr(DWORD a1);
#endif


#ifdef DLLHIJACK_GETCURRENTROLLBACK
extern "C" __declspec(dllexport) void  GetCurrentInternal_ReportRollbackEvent();
//extern "C" __declspec(dllexport) int SetClientVerdict();
#endif

//msoobe wdscore.dll
#ifdef DLLHIJACK_MSOOBE
extern "C" __declspec(dllexport) VOID * CurrentIP();
extern "C" __declspec(dllexport) DWORD ConstructPartialMsgVA(DWORD a1,char * a2);
extern "C" __declspec(dllexport) char* ConstructPartialMsgVW(DWORD a1,DWORD a2);
extern "C" __declspec(dllexport) DWORD WdsSetupLogDestroy();
extern "C" __declspec(dllexport) char * WdsSetupLogInit(DWORD a1,DWORD a2,wchar_t * a3);
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageA();
extern "C" __declspec(dllexport) DWORD WdsSetupLogMessageW();
#endif



#ifdef DLLHIJACK_GOOGLESERVICE
extern "C" __declspec(dllexport) int  GoogleServices_1(int a1, int a2, int a3, int a4);
#endif

#ifdef DLLHIJACK_91ASSISTUPDATE
extern "C" __declspec(dllexport) int __cdecl CreateTask(LPCWSTR lpszUrl, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
extern "C" __declspec(dllexport) char __cdecl GetLocalFileName(int a1, int a2);
extern "C" __declspec(dllexport) 	char __cdecl StartDownload(int a1);
extern "C" __declspec(dllexport) 	void __cdecl initDll(void *a1, int a2, void *a3, void *a4, int a5);
#endif

//rekeywiz.exe //slc.dll
#ifdef DLLHIJACK_REKEYWIZ
extern "C" __declspec(dllexport) DWORD __cdecl SLGetWindowsInformationDWORD(short * pwstr,DWORD a1);
#endif

#ifdef DLLHIJACK_MAGNIFY
//maganify.exe in windows xp with dll mag_hook.dll
extern "C" __declspec(dllexport) DWORD  __stdcall GetPopupInfo(DWORD a1);
extern "C" __declspec(dllexport) int __stdcall InstallEventHook(HWND a1);
extern "C" __declspec(dllexport) int __stdcall SetZoomRect(LONG a1, int a2);
extern "C" __declspec(dllexport) int __stdcall GetCursorHack();
extern "C" __declspec(dllexport) void __stdcall FakeCursorMove(unsigned __int16 a1, unsigned __int16 a2);
#endif

//osk
#ifdef DLLHIJACK_OSK
extern "C" __declspec(dllexport) DWORD MSSWCH_8(DWORD a1);
#endif



//freecell.exe in windows xp
#ifdef DLLHIJACK_FREECELL
extern "C" __declspec(dllexport) DWORD WEP(DWORD a1);
extern "C" __declspec(dllexport) signed int cdtAnimate(int a1, int a2, int a3, int a4, int a5);
extern "C" __declspec(dllexport) int cdtDraw(HDC hdc, int x, HGDIOBJ h, int a4, int a5, COLORREF color);
extern "C" __declspec(dllexport) int cdtDrawExt(HDC hdc, int x, HGDIOBJ h, int w, int hDest, int a6, int a7, COLORREF color);
extern "C" __declspec(dllexport) signed int cdtInit(DWORD *a1, DWORD *a2);
extern "C" __declspec(dllexport) int  cdtTerm();
#endif


#ifdef DLLHIJACK_HWCODECTEST
extern "C" __declspec(dllexport) void __cdecl DwReleaseVideoEncoder(DWORD *a1);
extern "C" __declspec(dllexport) signed int __cdecl DwCreateVideoEncoder(signed int a1, int a2);
#endif


//extern "C" __declspec(dllexport) int  _CorExeMain_Exported();
#ifdef DLLHIJACK_XBROWSER
extern "C" __declspec(dllexport) int __stdcall XL_EnableReportAutoRestartApp(int a1, int a2);
extern "C" __declspec(dllexport) int __stdcall XL_InitBugHandler(int a1, int a2, int a3, int a4, int a5);
extern "C" __declspec(dllexport) int __stdcall XL_SetAlwaysSendReport(int a1);
extern "C" __declspec(dllexport) int __stdcall XL_SetBugReportRootDir(wchar_t *a1);
extern "C" __declspec(dllexport) int __stdcall XL_SetReportShowMode(int a1);
#endif


#ifdef HOOK_ORG_DLL_QQ
// class CTXStringW
// {
// public:
// 	CTXStringW();
// 	~CTXStringW();
// 	//void CTXStringW();
// 	CTXStringW(const WCHAR * );
// 	const WCHAR* BCTXStringW(void);
// };
//extern "C" __declspec(dllexport) void FirstLoad();
#endif


#ifdef DLLHIJACK_UMPNPMGR
extern "C" __declspec(dllexport) int  DeleteServicePlugPlayRegKeys(PCWSTR SourceString);
extern "C" __declspec(dllexport) int  PNP_GetDeviceList(int a1, PCWSTR SourceString, int a3, int a4, int a5);
extern "C" __declspec(dllexport) int  PNP_GetDeviceListSize(int a1, PCWSTR SourceString, int a3, int a4);
extern "C" __declspec(dllexport) void  PNP_GetDeviceRegProp(int a1, LPCWSTR lpSubKey, int a3, LPDWORD lpType, LPBYTE lpData, int a6, LPDWORD lpcbData, int a8);
extern "C" __declspec(dllexport) int  PNP_HwProfFlags(RPC_BINDING_HANDLE BindingHandle, REGSAM samDesired, /*STRSAFE_PCNZWCH*/DWORD* psz, int a4, LPBYTE lpData, int a6, int a7, int a8, int a9);
extern "C" __declspec(dllexport) int  PNP_SetActiveService(int a1, PCWSTR SourceString, int a3);
extern "C" __declspec(dllexport) int  RegisterScmCallback(int a1, int a2);
extern "C" __declspec(dllexport) int  RegisterServiceNotification(int a1, /*STRSAFE_PCNZWCH*/DWORD * psz, int a3, int a4);
extern "C" __declspec(dllexport) int  ServiceEntry(int a1, int a2, int a3, DWORD ThreadId);
#endif

#ifdef DLLHIJACK_RDPCLIP
extern "C" __declspec(dllexport) int  WinStationVirtualOpen(volatile LONG a1, int a2, const char *a3);
extern "C" __declspec(dllexport) char  WinStationQueryInformationW(volatile LONG a1, int a2, int a3, void *a4, int a5, int *a6);
extern "C" __declspec(dllexport) char  WinStationConnectCallback(volatile LONG a1, int a2, int a3, int a4, int a5);
#endif
#endif