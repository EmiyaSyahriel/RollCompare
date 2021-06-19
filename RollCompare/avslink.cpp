#include <Windows.h>
#include <avisynth.h>
#include <vector>
#include "avslink.h"
#include "RollCompare.h"

const AVS_Linkage* AVS_linkage = 0;

AVSValue __cdecl Create_SimpleSample(AVSValue args, void* user_data, IScriptEnvironment* env) {
	std::vector<PClip> clips;

	clips.push_back(args[0].AsClip());
	for (int i = 0; i < args[3].ArraySize(); i++) clips.push_back(args[3][i].AsClip());

	return new RollCompare(args[1].AsString(), args[2].AsInt(), clips, env);
}

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment * env, const AVS_Linkage* const vectors) {

	AVS_linkage = vectors;
	env->AddFunction("PSW_RollCompare", "c[dir]s[speed]i[clip1]c+", Create_SimpleSample, 0);
	return "Roll Compare";
}