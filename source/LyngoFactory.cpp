#include "public.sdk/source/main/pluginfactoryvst3.h"

#include "LyngoController.h"
#include "LyngoProcessor.h"
#include "LyngoUids.h"
#include "LyngoVersion.h"

#define stringPluginName "Lyngo Acoustic Dispersion"

bool InitModule()   { return true; }
bool DeinitModule() { return true; }

using namespace Steinberg::Vst;

BEGIN_FACTORY_DEF("Vox Audio Labs",
				  "http://www.voxaudiolabs.com",
				  "mailto:voxaudiolabs@gmail.com")

	DEF_CLASS2(INLINE_UID_FROM_FUID(LyngoProcessorUid)
			   , PClassInfo::kManyInstances
			   , kVstAudioEffectClass
			   , stringPluginName
			   , Vst::kDistributable
			   , "Lyngo Acoustic Dispersion"
			   , FULL_VERSION_STR							// Plug-in version (to be changed)
			   , kVstVersionString
			   , LyngoProcessor::createInstance)

	DEF_CLASS2(INLINE_UID_FROM_FUID(LyngoControllerUid)
			   , PClassInfo::kManyInstances
			   , kVstComponentControllerClass
			   , stringPluginName "LyngoController"	
			   , 0
			   , ""
			   , FULL_VERSION_STR							// Plug-in version (to be changed)
			   , kVstVersionString
			   , LyngoController::createInstance)

	END_FACTORY


