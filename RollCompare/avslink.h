#pragma once
#include <Windows.h>
#include <avisynth.h>
#include <vector>
#include "RollCompare.h"

AVSValue __cdecl Create_SimpleSample(AVSValue args, void* user_data, IScriptEnvironment* env);

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment * env, const AVS_Linkage* const vectors);