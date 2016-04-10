#pragma once

#include "external\vst3\pluginterfaces\base\funknown.h"

// Parameters
enum class Parameters
{
	FStart		  = 0,
	FEnd		  = 1,
	IrMode		  = 2,
	IrLinear	  = 3,
	IrExponential = 4,
	IrSmart		  = 5,
};

// Message Uids
namespace Message
{
	static const Steinberg::FIDString Data			 = "Data";
	static const Steinberg::FIDString SampleRate	 = "SampleRate";
	static const Steinberg::FIDString IrTime		 = "IrTime";
}

static const unsigned BlockSize				 = 256;
static const unsigned ZPadBlockSize			 = 2 * BlockSize;
static const unsigned IrTimeDomainSize		 = BlockSize + 1;
static const unsigned IrFreqDomainSize		 = (ZPadBlockSize / 2) + 1;

// unique class ids (generate with guidgen.exe)
static const Steinberg::FUID LyngoProcessorUid (0x56441971, 0x6c6c, 0x4337, 0x97BB);
static const Steinberg::FUID LyngoControllerUid(0x65bd40b1, 0xc7b3, 0x4445, 0x81C1);

