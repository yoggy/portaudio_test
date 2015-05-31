#ifdef WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdio.h>
#include <locale.h>

#include <iostream>
#include <string>
#include <sstream>

#include "PA.h"

//////////////////////////////////////////////////////////////////
class PA_initializer_ {
public:
	PA_initializer_() {
		setlocale(LC_ALL, "");

		int err = Pa_Initialize();
		if (err != paNoError)
		{
			printf("ERROR: Pa_Initialize returned 0x%x\n", err);
		}
	};
	virtual ~PA_initializer_() {
		Pa_Terminate();
	};
};

PA_initializer_ pa_initializer_;

//////////////////////////////////////////////////////////////////
PADeviceInfo::PADeviceInfo() : idx_(-1), info_(NULL)
{

}

void PADeviceInfo::set_device_idx_(const int &idx)
{
	const PaDeviceInfo *p = Pa_GetDeviceInfo(idx);
	if (p == NULL) {
		printf("error : PADeviceInfo::set_device_idx() : Pa_GetDeviceInfo() error...idx=", idx);
		return;
	}

	this->idx_ = idx;
	this->info_ = p;
}

bool PADeviceInfo::is_valid_() const
{
	if (this->idx_ == -1 || this->info_ == NULL) return false;
	return true;
}

bool PADeviceInfo::is_asio() const
{
	if (Pa_GetHostApiInfo(info_->hostApi)->type == paASIO) return true;
	return false;
}

int PADeviceInfo::idx() const
{
	return this->idx_;
}

std::string PADeviceInfo::name() const
{
	if (!is_valid_()) return "";

#ifdef WIN32
	wchar_t name_w[MAX_PATH];
	char name_mb[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, info_->name, -1, name_w, MAX_PATH - 1);
	WideCharToMultiByte(CP_THREAD_ACP, 0, name_w, -1, name_mb, MAX_PATH - 1, NULL, NULL);
	return std::string(name_mb);
#else
	return std::string(info_->name);
#endif
}

std::string PADeviceInfo::host_ap() const
{
	if (!is_valid_()) return "none";
	return std::string(Pa_GetHostApiInfo(info_->hostApi)->name);
}

int PADeviceInfo::max_input() const
{
	if (!is_valid_()) return 0;
	return info_->maxInputChannels;
}

int PADeviceInfo::max_output() const
{
	if (!is_valid_()) return 0;
	return info_->maxOutputChannels;
}

PaTime PADeviceInfo::default_low_input_latency() const
{
	if (!is_valid_()) return 0;
	return info_->defaultLowInputLatency;
}

PaTime PADeviceInfo::default_low_output_latency() const
{
	if (!is_valid_()) return 0;
	return info_->defaultLowOutputLatency;
}

PaTime PADeviceInfo::default_high_input_latency() const
{
	if (!is_valid_()) return 0;
	return info_->defaultHighInputLatency;
}

PaTime PADeviceInfo::default_high_output_latency() const
{
	if (!is_valid_()) return 0;
	return info_->defaultHighOutputLatency;
}

#define GET_ASIO_LATENCY_VALUE() \
	long min_buf_size, max_buf_size, prefered_buf_size, buf_granularity; \
	PaAsio_GetAvailableBufferSizes(idx_, &min_buf_size, &max_buf_size, &prefered_buf_size, &buf_granularity); 

int PADeviceInfo::asio_min_buffer_size() const
{
	if (!is_valid_()) return 0;
	if (!is_asio()) return 0;

	GET_ASIO_LATENCY_VALUE();
	
	return min_buf_size;
}

int PADeviceInfo::asio_max_buffer_size() const
{
	if (!is_valid_()) return 0;
	if (!is_asio()) return 0;

	GET_ASIO_LATENCY_VALUE();

	return max_buf_size;
}

int PADeviceInfo::asio_prefereed_buffer_size() const
{
	if (!is_valid_()) return 0;
	if (!is_asio()) return 0;

	GET_ASIO_LATENCY_VALUE();

	return prefered_buf_size;
}


std::vector<int> PADeviceInfo::get_supported_sample_rates_(const PaStreamParameters *input_param, const PaStreamParameters *out_param) const
{
	static int sample_rates_[] = {
		8000, 11025, 22050, 44100, 48000, 96000, 192000, -1
	};

	PaError err;
	std::vector<int> rates;

	for (unsigned int i = 0; sample_rates_[i] > 0; i++) {
		err = Pa_IsFormatSupported(input_param, out_param, (double)sample_rates_[i]);
		if (err == paFormatIsSupported) {
			rates.push_back(sample_rates_[i]);
		}
	}

	return rates;
}

std::vector<int> PADeviceInfo::supported_input_sample_rate() const
{
	PaStreamParameters input_param;

	input_param.device = idx_;
	input_param.channelCount = info_->maxInputChannels;
	input_param.sampleFormat = paInt16;
	input_param.suggestedLatency = 0;
	input_param.hostApiSpecificStreamInfo = NULL;

	return get_supported_sample_rates_(&input_param, NULL);
}

std::vector<int> PADeviceInfo::supported_output_sample_rate() const
{
	PaStreamParameters output_param;

	output_param.device = idx_;
	output_param.channelCount = info_->maxOutputChannels;
	output_param.sampleFormat = paInt16;
	output_param.suggestedLatency = 0;
	output_param.hostApiSpecificStreamInfo = NULL;

	return get_supported_sample_rates_(NULL, &output_param);
}

std::string PADeviceInfo::supported_input_sample_rate_str() const
{
	std::stringstream ss;
	std::vector<int> rates = supported_input_sample_rate();
	for (unsigned int i = 0; i < rates.size(); ++i) {
		ss << rates[i] / 1000.0f; // kHz
		if (i < rates.size() - 1) ss << ", ";
	}
	return ss.str();
}

std::string PADeviceInfo::supported_output_sample_rate_str() const
{
	std::stringstream ss;
	std::vector<int> rates = supported_output_sample_rate();
	for (unsigned int i = 0; i < rates.size(); ++i) {
		ss << rates[i] / 1000.0f; // kHz
		if (i < rates.size() - 1) ss << ", ";
	}
	return ss.str();
}

bool PADeviceInfo::has_input()
{
	if (max_input() > 0) return true;
	return false;
}

bool PADeviceInfo::has_output()
{
	if (max_output() > 0) return true;
	return false;
}

std::string PADeviceInfo::str()
{
	std::stringstream ss;
	ss << "== idx=" << idx() << std::endl;
	ss << "  name    = " << name() << std::endl;
	ss << "  host_ap = " << host_ap() << std::endl;

	if (has_input()) {
		ss << "  [in]  max_input  = " << max_input() << std::endl;
		ss << "        default_low_input_latency(s)   = " << default_low_input_latency() << std::endl;
		ss << "        default_high_input_latency(s)  = " << default_high_input_latency() << std::endl;
		ss << "        freq(kHz) = " << supported_input_sample_rate_str() << std::endl;
	}

	if (has_output()) {
		ss << "  [out] max_output = " << max_output() << std::endl;
		ss << "        default_low_output_latency(s)  = " << default_low_output_latency() << std::endl;
		ss << "        default_high_output_latency(s) = " << default_high_output_latency() << std::endl;
		ss << "        freq(kHz) = " << supported_output_sample_rate_str() << std::endl;
	}

	if (is_asio()) {
		ss << "  asio_min_buffer_size       = " << asio_min_buffer_size() << std::endl;
		ss << "  asio_max_buffer_size       = " << asio_max_buffer_size() << std::endl;
		ss << "  asio_prefereed_buffer_size = " << asio_prefereed_buffer_size() << std::endl;
	}

	return ss.str();
}

//////////////////////////////////////////////////////////////////

PA::PA() : input_device_idx_(-1), output_device_idx_(-1), stream_(NULL), stream_type_(PA_STREAM_TYPE_NONE), channels_(0)
{

}

PA::~PA()
{
	close();
}

bool PA::is_open() const
{
	if (stream_ != NULL) return true;
	return false;
}

int PA::stream_type() const
{
	return stream_type_;
}

int PA::input_device_idx() const
{
	return input_device_idx_;
}

int PA::output_device_idx() const
{
	return output_device_idx_;
}

int PA::channels() const
{
	return channels_;
}

PADeviceInfo PA::input_device_info() const
{
	PADeviceInfo info;

	if (is_open() == false) return PADeviceInfo();

	bool rv = get_device_info(input_device_idx_, info);
	if (rv == false) return PADeviceInfo();
	
	return info;
}

PADeviceInfo PA::output_device_info() const
{
	PADeviceInfo info;

	if (is_open() == false) return PADeviceInfo();

	bool rv = get_device_info(output_device_idx_, info);
	if (rv == false) return PADeviceInfo();

	return info;
}

bool PA::open_input(const int &dev_idx, const int &channels, const int &sampling_rate, const int &buf_size)
{
	PaError err;
	PaStreamParameters input_param;

	if (is_open()) return false;

	if (dev_idx < 0 || get_device_count() <= dev_idx) {
		printf("error :  PA::open_input() : out of index...dev_idx=%d", dev_idx);
		return false;
	}

	input_param.device = dev_idx;
	input_param.channelCount = channels;
	input_param.sampleFormat = paFloat32;
	input_param.suggestedLatency = Pa_GetDeviceInfo(input_param.device)->defaultLowInputLatency;
	input_param.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(
		&stream_,
		&input_param,
		NULL,
		sampling_rate,
		buf_size,  // frames_per_buffer
		paClipOff,
		record_callback_,
		this);

	if (err != paNoError) {
		printf("error :  PA::open_input() : Pa_OpenStream() failed...dev_idx=%d", dev_idx);
		close();
		return false;
	}

	err = Pa_StartStream(stream_);
	if (err != paNoError) {
		printf("error :  PA::open_input() : Pa_StartStream() failed...dev_idx=%d", dev_idx);
		close();
		return false;
	}

	stream_type_ = PA_STREAM_TYPE_INPUT;
	input_device_idx_ = dev_idx;
	channels_ = channels;

	PADeviceInfo info = input_device_info();
	printf("PA::open_input() : open device...dev_idx=%d, name=%s\n", input_device_idx_, info.name().c_str());

	return true;
}

bool PA::open_output(const int &dev_idx, const int &channels, const int &sampling_rate, const int &buf_size)
{
	PaError err;
	PaStreamParameters output_param;

	if (is_open()) return false;

	if (dev_idx < 0 || get_device_count() <= dev_idx) {
		printf("error :  PA::open_output() : out of index...dev_idx=%d", dev_idx);
		return false;
	}

	output_param.device = dev_idx;
	output_param.channelCount = channels;
	output_param.sampleFormat = paFloat32;
	output_param.suggestedLatency = Pa_GetDeviceInfo(output_param.device)->defaultLowOutputLatency;
	output_param.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(
		&stream_,
		NULL,
		&output_param,
		sampling_rate,
		buf_size,  // frames_per_buffer
		paClipOff,
		play_callback_,
		this);

	if (err != paNoError) {
		printf("error :  PA::open_output() : Pa_OpenStream() failed...dev_idx=%d", dev_idx);
		close();
		return false;
	}

	err = Pa_StartStream(stream_);
	if (err != paNoError) {
		printf("error :  PA::open_output() : Pa_StartStream() failed...dev_idx=%d", dev_idx);
		close();
		return false;
	}

	stream_type_ = PA_STREAM_TYPE_OUTPUT;
	output_device_idx_ = dev_idx;
	channels_ = channels;

	PADeviceInfo info = output_device_info();
	printf("PA::open_output() : open device...dev_idx=%d, name=%s\n", output_device_idx_, info.name().c_str());

	return true;
}

bool PA::open_wire(const int &input_dev_idx, const int &output_dev_idx, const int &sampling_rate, const int &buf_size)
{
	PaError err;
	PaStreamParameters input_param;
	PaStreamParameters output_param;

	if (is_open()) return false;

	if (input_dev_idx < 0 || get_device_count() <= input_dev_idx) {
		printf("error :  PA::open_wire() : out of index...input_dev_idx=%d", input_dev_idx);
		return false;
	}
	if (output_dev_idx < 0 || get_device_count() <= output_dev_idx) {
		printf("error :  PA::open_wire() : out of index...output_dev_idx=%d", output_dev_idx);
		return false;
	}

	input_param.device = input_dev_idx;
	input_param.channelCount = 2;
	input_param.sampleFormat = paFloat32;
	input_param.suggestedLatency = Pa_GetDeviceInfo(input_param.device)->defaultLowInputLatency;
	input_param.hostApiSpecificStreamInfo = NULL;

	output_param.device = output_dev_idx;
	output_param.channelCount = 2;
	output_param.sampleFormat = paFloat32;
	output_param.suggestedLatency = Pa_GetDeviceInfo(output_param.device)->defaultLowInputLatency;
	output_param.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(
		&stream_,
		&input_param,
		&output_param,
		sampling_rate,
		buf_size,  // frames_per_buffer
		paClipOff,
		wire_callback_,
		this);

	if (err != paNoError) {
		printf("error :  PA::open_wire() : Pa_OpenStream() failed...intput_dev_idx=%d, output_dev_idx=%d", input_dev_idx, output_dev_idx);
		close();
		return false;
	}

	err = Pa_StartStream(stream_);
	if (err != paNoError) {
		printf("error :  PA::open_wire() : Pa_StartStream() failed...intput_dev_idx=%d, output_dev_idx=%d", input_dev_idx, output_dev_idx);
		close();
		return false;
	}

	stream_type_ = PA_STREAM_TYPE_WIRE;
	input_device_idx_ = input_dev_idx;
	output_device_idx_ = output_dev_idx;
	channels_ = 2;

	PADeviceInfo in_info = this->input_device_info();
	PADeviceInfo out_info = this->output_device_info();
	printf("PA::open_input() : open device...input=%d:%s, output=%d:%s\n", 
		input_dev_idx, in_info.name().c_str(),
		output_dev_idx, out_info.name().c_str());

	return true;
}

void PA::close()
{
	if (is_open()) {
		Pa_StopStream(stream_);
		Pa_CloseStream(stream_);

		if (stream_type_ == PA_STREAM_TYPE_INPUT) {
			printf("PA::close() : close input device...in:%d:%s\n", input_device_idx_, input_device_info().name().c_str());
		}
		else if (stream_type_ == PA_STREAM_TYPE_OUTPUT) {
			printf("PA::close() : close output device...out:%d:%s\n", output_device_idx_, output_device_info().name().c_str());
		}
		else if (stream_type_ == PA_STREAM_TYPE_WIRE) {
			printf("PA::close() : close wire device...in:%d:%s, out:%d:%s\n", 
				input_device_idx_, input_device_info().name().c_str(),
				output_device_idx_, output_device_info().name().c_str());
		}

		stream_ = NULL;
		stream_type_ = PA_STREAM_TYPE_NONE;
		input_device_idx_ = -1;
		output_device_idx_ = -1;
		channels_ = 0;
	}
}

int PA::record_callback(const float *buf,
	unsigned long buf_size,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flag)
{
	printf("PA::record_callback(() buf_size=%d\n", buf_size);
	return paContinue;
}

int PA::play_callback(float *buf,
	unsigned long buf_size,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flag)
{
	printf("PA::play_callback() buf_size=%d\n", buf_size);
	return paContinue;
}

int PA::wire_callback(const float *buf,
	unsigned long buf_size,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flag)
{
	printf("PA::record_callback(() buf_size=%d\n", buf_size);
	return paContinue;
}

int PA::record_callback_(const void *input_buffer, void *output_buffer,
	unsigned long frames_per_buffer,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flag,
	void *user_data)
{
	PA *pa = (PA *)user_data;

	float *buf = (float*)input_buffer;
	unsigned long buf_size = frames_per_buffer * pa->channels();

	return pa->record_callback(buf, buf_size, time_info, status_flag);
}

int PA::play_callback_(const void *input_buffer, void *output_buffer,
	unsigned long frames_per_buffer,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flag,
	void *user_data)
{
	PA *pa = (PA *)user_data;

	float *buf = (float*)output_buffer;
	unsigned long buf_size = frames_per_buffer * pa->channels();

	return pa->play_callback(buf, buf_size, time_info, status_flag);
}

int PA::wire_callback_(const void *input_buffer, void *output_buffer,
	unsigned long frames_per_buffer,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flag,
	void *user_data)
{
	PA *pa = (PA *)user_data;

	float *in_buf = (float*)input_buffer;
	float *out_buf = (float*)output_buffer;
	unsigned long buf_size = frames_per_buffer * pa->channels();  // input is single channel...

	memcpy(out_buf, in_buf, buf_size * sizeof(float));

	return pa->wire_callback(in_buf, buf_size, time_info, status_flag);
}

int PA::get_default_input_device_idx()
{
	return Pa_GetDefaultInputDevice();
}

int PA::get_default_output_device_idx()
{
	return Pa_GetDefaultOutputDevice();
}

int PA::get_device_count()
{
	int num = Pa_GetDeviceCount();
	if (num < 0)
	{
		printf("error: PA::get_device_count() : err=0x%x\n", num);
		return 0;
	}
	return num;
}

bool PA::get_device_info(const int &idx, PADeviceInfo &info)
{
	int num = get_device_count();
	if (idx < 0 || num <= idx) {
		printf("error : PA::get_device_info() : out of index...idx=", idx);
		return false;
	}

	info.set_device_idx_(idx);

	return true;
}

std::vector<PADeviceInfo> PA::get_device_infos()
{
	int num = get_device_count();

	std::vector<PADeviceInfo> infos;
	for (int i = 0; i < num; ++i) {
		PADeviceInfo info;
		if (get_device_info(i, info)) {
			infos.push_back(info);
		}
	}

	return infos;
}

void PA::print_devices()
{
	std::vector<PADeviceInfo> infos = PA::get_device_infos();
	for (unsigned int i = 0; i < infos.size(); ++i) {
		printf("%s\n", infos[i].str().c_str());
	}
}