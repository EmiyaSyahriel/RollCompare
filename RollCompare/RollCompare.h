// RollingComparison.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <Windows.h>
#include <avisynth.h>
#include <iostream>
#include <vector>

class RollCompare : public GenericVideoFilter {
	int speed = 0;
	int pos = 0;

	enum class RollDir {
		/* Not comparing, showing original video */
		NONE,
		/* Move vertically, Horizontal slices */
		VERTICAL,
		/* Move horizontally, Vertical slices */
		HORIZONTAL
	};

	RollDir getRollDir(const char* name);

	std::vector<PClip> clips;
	RollDir currentDir;

	float lerp(float a, float b, float t);
	int lerpi(int a, int b, float t);
	unsigned char lerpb(unsigned char a, unsigned char b, float t);
	void transite(PVideoFrame dst, PVideoFrame a, PVideoFrame b, int x, int y, bool isOn);

public:
	RollCompare(const char* dir, int rpf, std::vector<PClip> clips, IScriptEnvironment* env);
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
};