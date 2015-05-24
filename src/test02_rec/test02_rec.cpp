#ifdef WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdio.h>
#include "../common/PA.h"

int main(int argc, char* argv[])
{
	PA pa;
	bool rv = pa.open_input(0, 1, paInt16, 44100, 64 * 5);
	if (rv == false) {
		printf("error : pa.open_input() failed...\n");
		return -1;
	}

	Sleep(3000);

	return 0;
}

