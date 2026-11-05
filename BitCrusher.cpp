#include "fmod_common.h"
#include "BitCrusher.h"
#include <cmath>
#include <malloc.h>
#include <cstring>

void BitCrusher::Init(FMOD_DSP_STATE* dsp_state)
{
	Reset(dsp_state);

	HoldSampleArray = new float[FMOD_MAX_CHANNEL_WIDTH];
	memset(HoldSampleArray, 0, FMOD_MAX_CHANNEL_WIDTH);

	LeftoverArray = new float[FMOD_MAX_CHANNEL_WIDTH];
	memset(LeftoverArray, 0, FMOD_MAX_CHANNEL_WIDTH);
}

void BitCrusher::Release()
{
	delete HoldSampleArray;
	HoldSampleArray = nullptr;

	delete LeftoverArray;
	LeftoverArray = nullptr;
}

void BitCrusher::Reset(FMOD_DSP_STATE* dsp_state)
{
	SampleRateReduction = 0;
	BitDepth = 32;
}

void BitCrusher::Read(float* inbuffer, float* outbuffer, unsigned int length, int channels)
{
	unsigned int BufferLength = length * channels;

	for (unsigned int i = 0; i < BufferLength; i += channels)
	{
		for (int currentChannel = 0; currentChannel < channels; currentChannel++)
		{
			int bufferIndex = i + currentChannel;
			float currentSample = BitReduction(inbuffer[bufferIndex]);
			currentSample = DownSample(currentSample, currentChannel);
			currentSample = Clamp(currentSample);
			outbuffer[bufferIndex] = currentSample;
		}
	}
}

float BitCrusher::BitReduction(float inputSample)
{
	float Result = 0.0f;

	float MaxValue = powf(2, floorf(BitDepth));
	// Shrink down to 0 - 1 range
	Result = (0.5f * inputSample + 0.5f);
	// Scale up by the maxium bits
	Result *= MaxValue;
	// Round to nearest bit
	Result = roundf(Result);
	// Invert scaling for output
	Result /= MaxValue;
	Result = 2 * Result - 1;

	return Result;
}

float BitCrusher::DownSample(float inputSample, int channel)
{
	if (LeftoverArray[channel] == 0) //Sample
	{
		LeftoverArray[channel] += SampleRateReduction;
		HoldSampleArray[channel] = inputSample;
		return inputSample;
	}
	else //Hold
	{
		LeftoverArray[channel]--;
		return HoldSampleArray[channel];
	}
}

float BitCrusher::Clamp(float inputSample)
{
	if (inputSample > 1)
	{
		return 1;
	}
	else if (inputSample < -1)
	{
		return -1;
	}
	else
	{
		return inputSample;
	}
}

