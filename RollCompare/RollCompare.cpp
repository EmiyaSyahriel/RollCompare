// RollingComparison.cpp : Defines the entry point for the application.
//

#include "RollCompare.h"
#include <avisynth.h>
#include <string.h>
#include <algorithm>

#define between(t,min,max) (t < max && t > min)
#define between2(t,mina,maxa,minb,maxb) ((t < maxa && t > mina) || (t < maxb && t > minb))

RollCompare::RollDir RollCompare::getRollDir(const char* name)
{
	std::string n = name;
	std::transform(n.begin(), n.end(), n.begin(), ::tolower);
	if (n == "v" || n == "y" || n == "vertical" || n == "vert") return RollDir::VERTICAL;
	else if (n == "h" || n == "x" || n == "horizontal" || n == "horz") return RollDir::HORIZONTAL;
	else return RollDir::NONE;
}

float RollCompare::lerp(float a, float b, float t) { return a + ((b - a) * t); }

int RollCompare::lerpi(int a, int b, float t) { return (int)lerp((float)a, (float)b, t); }

unsigned char RollCompare::lerpb(unsigned char a, unsigned char b, float t) { return max(min(lerpi(a, b, t),255),0); }

void RollCompare::transite(PVideoFrame dst, PVideoFrame a, PVideoFrame b, int x, int y, bool isOn)
{
	const unsigned char* _a = a->GetReadPtr();
	const unsigned char* _b = b->GetReadPtr();
	bool isW = dst->IsWritable();
	unsigned char* srcp = dst->GetWritePtr();

	int w = vi.width, h = vi.height;

	int colPerPx = vi.IsRGB32() ? 4 : 3;

	int pitch = dst->GetPitch();
	for (int p = 0; p < y; p++) { 
		srcp += pitch; 
		_a += pitch;
		_b += pitch;
	}
	for (int p = 0; p < x; p++) { 
		srcp += colPerPx;
		_a += colPerPx;
		_b += colPerPx;
	}
	for (int c = 0; c < colPerPx; c++) {
		*(srcp + c) = lerpb(*(_a + c), *(_b + c), isOn ? 1.0F : 0.0F);
	}
}

RollCompare::RollCompare(const char* dir, int rpf, std::vector<PClip> clips, IScriptEnvironment* env)
	:GenericVideoFilter(clips[0]), clips(clips), speed(rpf), currentDir(getRollDir(dir))
{
	VideoInfo firstvi = clips[0]->GetVideoInfo();
	int w = firstvi.width, h = firstvi.height;
	if (!(firstvi.IsRGB32() || firstvi.IsRGB24())) {
		env->ThrowError("This plugin only supports RGB[24|32] color space.");
		return;
	}
	for (PClip clip : clips) {
		VideoInfo mvi = clip->GetVideoInfo();
		if (mvi.width != w && mvi.height != h) {
			env->ThrowError("Clip sizes different");
			break;
		}
	}
}

PVideoFrame __stdcall RollCompare::GetFrame(int n, IScriptEnvironment* env)
{
	try
	{
		if (currentDir != RollDir::NONE) {
			bool isHorz = currentDir == RollDir::HORIZONTAL;

			PVideoFrame frame = env->NewVideoFrame(vi);

			int sliceSize = (currentDir == RollDir::HORIZONTAL ? vi.width : vi.height) / (int)clips.size();
			pos = isHorz ? ((n * speed) % vi.width) : ((n * -speed) % vi.height);

			int colPerPx = vi.IsRGB32() ? 4 : 3;

			for (int ci = 0; ci < (int)clips.size(); ci++) {
				PVideoFrame clip_a = clips[0]->GetFrame(n, env);
				PVideoFrame clip_b = clips[ci]->GetFrame(n, env);

				const unsigned char* _a = clip_a->GetReadPtr();
				const unsigned char* _b = clip_b->GetReadPtr();
				bool isW = frame->IsWritable();
				unsigned char* srcp = frame->GetWritePtr();
				int pitch = frame->GetPitch();

				int startPosA = ((sliceSize * ci) + pos);
				int endPosA = ((sliceSize * (ci + 1)) + pos);

				int startPosB = startPosA;
				int endPosB = endPosA;

				if (isHorz) {

					startPosB -= vi.width;
					endPosB -= vi.width;
				    startPosA *= colPerPx;
					endPosA *= colPerPx;
					startPosB *= colPerPx;
					endPosB *= colPerPx;
				}
				else {
					startPosB += vi.height;
					endPosB += vi.height;
				}

				for (int y = 0; y < vi.height; y++) {
					for (int x = 0; x < pitch; x += colPerPx) {
						bool isOn = isHorz ? 
							between2(x, startPosA, endPosA, startPosB, endPosB):
							between2(y, startPosA, endPosA, startPosB, endPosB);
						//transite(frame, clip_a, clip_b, x, y, isOn);
						for (int c = 0; c < colPerPx; c++) {
							*(srcp + x + c) = lerpb(*(_a + x + c), *(_b + x + c), isOn ? 1.0F : 0.0F);
						}
					}
					srcp += pitch;
					_a += pitch;
					_b += pitch;
				}
			}
			return frame;

		}
	}
	catch (const std::exception& err)
	{
		env->ThrowError(err.what());
	}
	return child->GetFrame(n, env);
}
