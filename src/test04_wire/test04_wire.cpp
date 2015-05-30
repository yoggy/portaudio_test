#ifdef WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdio.h>
#include "../common/PA.h"

class TestWire : public PA {
public:
	TestWire() : PA() {
	}

protected:
	virtual int wire_callback(const short *buf,
		unsigned long buf_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag) {

		int max_val = 0;
		for (unsigned int i = 0; i < buf_size; ++i) {
			int val = abs(buf[i]);
			if (val > max_val) {
				max_val = val;
			}
		}

		printf("buf_size=%d, max_val=%d\n", buf_size, max_val);

		return paContinue;
	}
};

int main(int argc, char* argv[])
{
	TestWire pa;
	bool rv = pa.open_wire(0, 0, 44100, 256);
	if (rv == false) {
		printf("error : pa.open_wire() failed...\n");
		return -1;
	}

	Sleep(3 * 1000);

	pa.close();

	return 0;
}

