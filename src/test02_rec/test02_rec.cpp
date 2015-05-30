#ifdef WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdio.h>
#include "../common/PA.h"

class TestRecord : public PA {
public:
	TestRecord() : PA() {
	}

protected:
	virtual int record_callback(const void *input_buffer,
		unsigned long frames_per_buffer,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag) {

		printf("%d\n", frames_per_buffer);
		return paContinue;
	}
};

int main(int argc, char* argv[])
{
	TestRecord pa;
	bool rv = pa.open_input(1, 1, paInt16, 44100, 64 * 100);
	if (rv == false) {
		printf("error : pa.open_input() failed...\n");
		return -1;
	}

	Sleep(10 * 1000);

	return 0;
}

