#pragma once

#ifdef _DEBUG
#define VM_EVASION_DELAY 3
#else
#define VM_EVASION_DELAY 0
#endif

char* cpuBrand(char* strCpuBrand);



class VM {

public:

	static int checkSandbox();

	static int checkTickCount();

	static int delay(int seconds);
};