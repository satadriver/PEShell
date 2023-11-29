#pragma once


#define VM_EVASION_DELAY 600


char* cpuBrand(char* strCpuBrand);

class VM {

public:

	static int checkVM();

	static int checkTickCount();

	static int delay(int seconds);
};