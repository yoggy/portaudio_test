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
	virtual int record_callback(const float *buf,
		unsigned long buf_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag) {

		float max_val = 0;
		for (unsigned int i = 0; i < buf_size; ++i) {
			float val = abs(buf[i]);
			if (val > max_val) {
				max_val = val;
			}
		}

		printf("buf_size=%d, max_val=%f\n", buf_size, max_val);

		return paContinue;
	}
};

int main(int argc, char* argv[])
{
	TestRecord pa;
	bool rv = pa.open_input(1, 1, 44100, 256);
	if (rv == false) {
		printf("error : pa.open_input() failed...\n");
		return -1;
	}

	Sleep(10 * 1000);

	pa.close();

	return 0;
}

