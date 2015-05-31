#pragma once

// portaudio
#define PA_USE_ASIO 1
#include <portaudio.h>
#ifdef PA_USE_ASIO
#include "pa_asio.h"
#endif

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "portaudio_x86D.lib")
#else
#pragma comment(lib, "portaudio_x86.lib")
#endif
#endif

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "portaudio_x64D.lib")
#else
#pragma comment(lib, "portaudio_x64.lib")
#endif
#endif

#include <vector>

class PADeviceInfo {
public:
	PADeviceInfo();

	int idx() const;

	bool is_asio() const;

	std::string name() const;
	std::string host_ap() const;
	int max_input() const;
	int max_output() const;

	PaTime default_low_input_latency() const;
	PaTime default_low_output_latency() const;
	PaTime default_high_input_latency() const;
	PaTime default_high_output_latency() const;

	int asio_min_buffer_size() const;
	int asio_max_buffer_size() const;
	int asio_prefereed_buffer_size() const;

	std::vector<int> supported_input_sample_rate() const;
	std::vector<int> supported_output_sample_rate() const;

	std::string supported_input_sample_rate_str() const;
	std::string supported_output_sample_rate_str() const;

	bool has_input();
	bool has_output();

	std::string str();

protected:
	void set_device_idx_(const int &idx);
	bool is_valid_() const;
	std::vector<int> get_supported_sample_rates_(const PaStreamParameters *inputParameters, const PaStreamParameters *outputParameters) const;

protected:
	int idx_;
	const PaDeviceInfo *info_;
	friend class PA;
};

#define PA_STREAM_TYPE_NONE   0
#define PA_STREAM_TYPE_OUTPUT 1
#define PA_STREAM_TYPE_INPUT  2
#define PA_STREAM_TYPE_WIRE   3

class PA
{
public:
	PA();
	virtual ~PA();
	
	bool is_open() const;
	int stream_type() const;
	int input_device_idx() const;
	int output_device_idx() const;
	PADeviceInfo input_device_info() const;
	PADeviceInfo output_device_info() const;
	int channels() const;
	
	bool open_input(const int &dev_idx, const int &channels, const int &sampling_rate, const int &buf_size);
	bool open_output(const int &dev_idx, const int &channels, const int &sampling_rate, const int &buf_size);
	bool open_wire(const int &input_dev_idx, const int &output_dev_idx, const int &sampling_rate, const int &buf_size);

	void close();

	static int get_default_input_device_idx();
	static int get_default_output_device_idx();
	static bool get_device_info(const int &idx, PADeviceInfo &info);
	static std::vector<PADeviceInfo> get_device_infos();
	static int get_device_count();
	static void print_devices();

protected:
	virtual int record_callback(const float *buf,
		unsigned long buf_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag);

	virtual int play_callback(float *buf,
		unsigned long buf_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag);

	virtual int wire_callback(const float *buf,
		unsigned long buf_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag);

	static int record_callback_(const void *input_buffer, void *output_buffer,
		unsigned long frames_per_buffer,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag,
		void *user_data);

	static int play_callback_(const void *input_buffer, void *output_buffer,
		unsigned long frames_per_buffer,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag,
		void *user_data);

	static int wire_callback_(const void *input_buffer, void *output_buffer,
		unsigned long frames_per_buffer,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flag,
		void *user_data);

protected:
	int input_device_idx_;
	int output_device_idx_;
	PaStream *stream_;
	int stream_type_;
	int channels_;
	
};

