#pragma once

#include <iostream>
#include <windows.h>

using namespace std;

class FakeFilename {
public:
	static int fakefn(string srcfn,string surfix,string dstfn);
};