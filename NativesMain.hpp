#pragma once

namespace plugin_natives
{
#ifdef PLUGIN_NATIVES_HAS_FUNC
	std::list<NativeFuncBase *> *
		NativeFuncBase::all_ = 0;
#endif

#ifdef PLUGIN_NATIVES_HAS_HOOK
	static bool
		gPluginNativesInit = true;

	std::list<NativeHookBase *> *
		NativeHookBase::all_ = 0;
#endif

	int AmxLoad(AMX * amx)
	{
		int
			ret = 0;
#ifdef PLUGIN_NATIVES_HAS_FUNC
		if (NativeFuncBase::all_)
		{
			// Need a sentinel because of:
			//   
			//   https://github.com/Zeex/sampgdk/issues/188
			//   
			// Otherwise we could do:
			//   
			//   curNative.name = curFunc->name_;
			//   curNative.func = curFunc->native_;
			//   ret = amx_Register(amx, &curNative, 1);
			//   
			AMX_NATIVE_INFO
				curNative[2] = { {0, 0}, {0, 0} };
			for (NativeFuncBase * curFunc : *NativeFuncBase::all_)
			{
				Log(LogLevel::INFO, "Registering native %s", curFunc->name_);
				curNative[0].name = curFunc->name_;
				curNative[0].func = curFunc->native_;
				ret = amx_Register(amx, curNative, -1);
			}
		}
#endif
#ifdef PLUGIN_NATIVES_HAS_HOOK
		if (gPluginNativesInit)
		{
			gPluginNativesInit = false;
			if (NativeHookBase::all_)
			{
				AMX_NATIVE
					curNative = 0;
				for (NativeHookBase * curFunc : *NativeHookBase::all_)
				{
					curNative = sampgdk_FindNative(curFunc->name_);
					if (curNative)
					{
						Log(LogLevel::INFO, "Hooking native %s: %p -> %p", curFunc->name_, (void *)curNative, (void *)curFunc->replacement_);
						curFunc->hook_.Install((void *)curNative, (void *)curFunc->replacement_);
					}
					else
					{
						Log(LogLevel::INFO, "Hooking native %s (NOT FOUND)", curFunc->name_);
					}
				}
			}
		}
#endif
		return ret;
	}
};

