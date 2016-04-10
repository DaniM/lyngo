#include "public.sdk/source/vst/vst2wrapper/vst2wrapper.h"
#include "LyngoUids.h"

::AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return Steinberg::Vst::Vst2Wrapper::create(GetPluginFactory()
											   , LyngoProcessorUid
											   , 'GnV2'
											   , audioMaster);
}
