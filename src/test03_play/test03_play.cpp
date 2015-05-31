#ifdef WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdio.h>
#include "../common/PA.h"

#include <vector>

class TestPlay : public PA {
protected:
	int idx_;
	std::vector<float> sin_table_;

public:
	TestPlay() : PA() , idx_(0) {
		float f = 440.0;
		int size = (int)((1.0 / f) * 44100.0);
		sin_table_.resize(size);
		for (int i = 0; i < size; ++i) {
			float p = (float)i / size;
			float th = p *  2 * 3.14159f;
			sin_table_[i] = (float)sin(th);
		}
	}

protected:
	virtual int play_callback(float *buf,
		unsigned long buf_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag) {

		for (unsigned int i = 0; i < buf_size; i+=2) {
			buf[i] = sin_table_[idx_];
			buf[i + 1] = sin_table_[idx_];
			idx_ = idx_ >= (int)sin_table_.size() ? 0 : idx_ + 1;
		}

		printf("buf_size=%d, time_info->outputBufferDacTime=%f\n", buf_size, time_info->outputBufferDacTime);

		return paContinue;
	}
};

int main(int argc, char* argv[])
{
	TestPlay pa;
	bool rv = pa.open_output(1, 2, 44100, 256);
	if (rv == false) {
		printf("error : pa.open_output() failed...\n");
		return -1;
	}

	Sleep(5 * 1000);

	pa.close();

	return 0;
}

