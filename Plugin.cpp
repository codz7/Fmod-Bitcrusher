#include <string.h>
#include "fmod.hpp"
#include "BitCrusher.h"

//https://www.fmod.com/docs/2.03/api/plugin-api-dsp.html
//https://www.fmod.com/docs/2.03/api/white-papers-dsp-plugin-api.html

extern "C" {
	F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription();
};

//Callbacks
FMOD_RESULT F_CALLBACK Create(FMOD_DSP_STATE* state);
FMOD_RESULT F_CALLBACK Release(FMOD_DSP_STATE* state);
FMOD_RESULT F_CALLBACK Reset(FMOD_DSP_STATE* state);
FMOD_RESULT F_CALLBACK Read(FMOD_DSP_STATE* state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels);
FMOD_RESULT F_CALLBACK ShouldIProcess(FMOD_DSP_STATE* state, FMOD_BOOL inputsidle, unsigned int length, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE speakermode);


FMOD_RESULT F_CALLBACK SetIntegerParameter(FMOD_DSP_STATE* dsp_state, int index, int value);
FMOD_RESULT F_CALLBACK GetIntegerParameter(FMOD_DSP_STATE* dsp_state, int index, int* value, char* valuestr);

/*
FMOD_RESULT F_CALLBACK Process(FMOD_DSP_STATE* state, unsigned int length, const FMOD_DSP_BUFFER_ARRAY* inbufferarray, FMOD_DSP_BUFFER_ARRAY* outbufferarray, FMOD_BOOL inputsidle, FMOD_DSP_PROCESS_OPERATION op);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspsetparamfloat(FMOD_DSP_STATE* dsp_state, int index, float value);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspsetparamint(FMOD_DSP_STATE* dsp_state, int index, int value);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspsetparambool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL value);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspsetparamdata(FMOD_DSP_STATE* dsp_state, int index, void* data, unsigned int length);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspgetparamfloat(FMOD_DSP_STATE* dsp_state, int index, float* value, char* valuestr);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspgetparamint(FMOD_DSP_STATE* dsp_state, int index, int* value, char* valuestr);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspgetparambool(FMOD_DSP_STATE* dsp_state, int index, FMOD_BOOL* value, char* valuestr);
FMOD_RESULT F_CALLBACK FMOD_Gain_dspgetparamdata(FMOD_DSP_STATE* dsp_state, int index, void** value, unsigned int* length, char* valuestr);
FMOD_RESULT F_CALLBACK FMOD_Gain_shouldiprocess(FMOD_DSP_STATE* dsp_state, FMOD_BOOL inputsidle, unsigned int length, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE speakermode);
FMOD_RESULT F_CALLBACK FMOD_Gain_sys_register(FMOD_DSP_STATE* dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Gain_sys_deregister(FMOD_DSP_STATE* dsp_state);
FMOD_RESULT F_CALLBACK FMOD_Gain_sys_mix(FMOD_DSP_STATE* dsp_state, int stage);
*/

static FMOD_DSP_PARAMETER_DESC BitDepthParameter;
static FMOD_DSP_PARAMETER_DESC SamplingRateParameter;

FMOD_DSP_PARAMETER_DESC* PluginParameterDescriptions[2] =
{
	&BitDepthParameter,
	&SamplingRateParameter
};

FMOD_DSP_DESCRIPTION BitCrusherPluginDescription =
{
	FMOD_PLUGIN_SDK_VERSION,
	"BitCrusher",		// name
	0x00000001,			// plug-in version
	1,					// number of input buffers to process
	1,					// number of output buffers to process
	Create,
	Release,
	Reset,
	Read,
	0,					// Process
	0,					// SetPosition
	2,					// number of parameter descriptions
	PluginParameterDescriptions,
	0,					// Setparamfloat,
	SetIntegerParameter,
	0,					// SetParambool,
	0,					// SetParamdata,
	0,					// Getparamfloat,
	GetIntegerParameter,
	0,					// GetParambool,
	0,					// GetParamdata,
	ShouldIProcess,
	0,					// UserData
	0,					// Sys_register,
	0,					// Sys_deregister,
	0,					// Sys_mix
};

extern "C"
{
	F_EXPORT FMOD_DSP_DESCRIPTION* F_CALL FMODGetDSPDescription()
	{
		FMOD_DSP_INIT_PARAMDESC_INT(BitDepthParameter, "BitDepth", " BitDepth", "Number of bits to quantize to", 1, 32, 32, false, nullptr);
		FMOD_DSP_INIT_PARAMDESC_INT(SamplingRateParameter, "SamplingRateReduction", " Factor", "How many samples to skip", 0, 1000, 0, false, nullptr);
		return &BitCrusherPluginDescription;
	}
}

FMOD_RESULT F_CALLBACK Create(FMOD_DSP_STATE* state)
{
	BitCrusher* bitCrusher = (BitCrusher*)FMOD_DSP_ALLOC(state, sizeof(BitCrusher));
	bitCrusher->Init(state);
	state->plugindata = bitCrusher;
	if (!state->plugindata)
	{
		return FMOD_ERR_MEMORY;
	}
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK Release(FMOD_DSP_STATE* state)
{
	BitCrusher* bitCrusher = (BitCrusher*)state->plugindata;
	bitCrusher->Release();
	FMOD_DSP_FREE(state, state->plugindata);

	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK Reset(FMOD_DSP_STATE* state)
{
	BitCrusher* bitCrusher = (BitCrusher*)state->plugindata;
	bitCrusher->Reset(state);
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK Read(FMOD_DSP_STATE* state, float* inbuffer, float* outbuffer, unsigned int length, int inchannels, int* outchannels)
{
	BitCrusher* bitCrusher = (BitCrusher*)state->plugindata;
	bitCrusher->Read(inbuffer, outbuffer, length, inchannels);
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK ShouldIProcess(FMOD_DSP_STATE* state, FMOD_BOOL inputsidle, unsigned int length, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE speakermode)
{
	if (inputsidle)
	{
		return FMOD_ERR_DSP_DONTPROCESS;
	}

	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK SetIntegerParameter(FMOD_DSP_STATE* dsp_state, int index, int value)
{
	BitCrusher* state = (BitCrusher*)dsp_state->plugindata;

	switch ((BitCrushParameterIndex)index)
	{
	case BitCrushParameterIndex::eBitDepth:
		state->BitDepth = value;
		return FMOD_OK;
	case BitCrushParameterIndex::eSampleRateReduction:
		state->SampleRateReduction = value;
		return FMOD_OK;
	}

	return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT F_CALLBACK GetIntegerParameter(FMOD_DSP_STATE* dsp_state, int index, int* value, char* valuestr)
{
	BitCrusher* state = (BitCrusher*)dsp_state->plugindata;

	switch ((BitCrushParameterIndex)index)
	{
		case BitCrushParameterIndex::eBitDepth:
			*value = state->BitDepth;
			return FMOD_OK;
		case BitCrushParameterIndex::eSampleRateReduction:
			*value = state->SampleRateReduction;
			return FMOD_OK;
	}

	return FMOD_ERR_INVALID_PARAM;
}
