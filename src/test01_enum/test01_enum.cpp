#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

#include "../common/PA.h"


int main(void)
{
	PA::print_devices();
}
