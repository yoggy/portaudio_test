#ifdef WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdio.h>
#include "../common/PA.h"

class TestPlay : public PA {
public:
	TestPlay() : PA() {
	}

protected:
	virtual int play_callback(float *buf,
		unsigned long buf_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag) {

		double st = time_info->outputBufferDacTime;
		double dt = 1.0 / 44100.0;

		for (unsigned int i = 0; i < buf_size; i+=2) {
			double t = st + i * dt;
			double th = t * 3.14159;
			buf[i] = (float)(cos(th * 440) );
			buf[i + 1] = (float)(cos(th * 220));
		}

		printf("buf_size=%d, time_info->outputBufferDacTime=%f\n", buf_size, time_info->outputBufferDacTime);

		return paContinue;
	}
};

int main(int argc, char* argv[])
{
	TestPlay pa;
	bool rv = pa.open_output(1, 2, 44100, 4410);
	if (rv == false) {
		printf("error : pa.open_output() failed...\n");
		return -1;
	}

	Sleep(5 * 1000);

	pa.close();

	return 0;
}

