#pragma once

#include <windows.h>
#include <iostream>

using namespace std;

class Section {
public:
	static string insertSection(int flag,const char *secname,const char srcfiles[16][256], int srcfilescnt,char * outname);
};