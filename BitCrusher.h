#pragma once
struct FMOD_DSP_STATE;

enum class BitCrushParameterIndex : char
{
	eBitDepth = 0,
	eSampleRateReduction = 1,
};

class BitCrusher
{
public:
	void Init(FMOD_DSP_STATE* dsp_state);
	void Release();
	void Reset(FMOD_DSP_STATE* dsp_state);

	void Read(float* inbuffer, float* outbuffer, unsigned int length, int channels);

	float BitReduction(float inputSample);
	float DownSample(float inputSample, int channel);
	float Clamp(float inputSample);

	int BitDepth;
	int SampleRateReduction;

	float* HoldSampleArray;
	float* LeftoverArray;
};

