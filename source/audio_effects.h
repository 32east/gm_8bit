#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>

namespace AudioEffects {
	enum {
		EFF_NONE,
		EFF_BITCRUSH,
		EFF_DESAMPLE,
		EFF_REVERB,
		EFF_VOICE_IN_MASK
	};

	void BitCrush(uint16_t* sampleBuffer, int samples, float quant, float gainFactor) {
		for (int i = 0; i < samples; i++) {
			// apply a low pass filter to remove high frequency noise
			sampleBuffer[i] = (sampleBuffer[i] + sampleBuffer[i-1]) / 2;
			// apply a distortion effect to emulate static noise
			sampleBuffer[i] = sampleBuffer[i] * 1.5;
			
			// apply a high pass filter to remove low frequency noise
			sampleBuffer[i] = sampleBuffer[i] - (sampleBuffer[i-1] / 2);
			
			// limit the amplitude to prevent clipping
			if (sampleBuffer[i] > 32767) {
				sampleBuffer[i] = 32767;
			} else if (sampleBuffer[i] < -32767) {
				sampleBuffer[i] = -32767;
			}
		}
	}

	static uint16_t tempBuf[10 * 1024];
	void Desample(uint16_t* inBuffer, int& samples, int desampleRate = 2) {
		assert(samples / desampleRate + 1 <= sizeof(tempBuf));
		int outIdx = 0;
		for (int i = 0; i < samples; i++) {
			if (i % desampleRate == 0) continue;

			tempBuf[outIdx] = inBuffer[i];
			outIdx++;
		}
		std::memcpy(inBuffer, tempBuf, outIdx * 2);
		samples = outIdx;
	}

	void Reverb(uint16_t* sampleBuffer, int samples, float decay, float density) {
		for (int i = 0; i < samples; i++) {
			int delay = i * decay;
			if (i + delay >= samples) break;
			sampleBuffer[i + delay] += sampleBuffer[i] * density;
		}
	}

	void VoiceInMask(uint16_t* sampleBuffer, int samples, float lowPassFreq) {
		for (int i = 0; i < samples; i++) {
			if (sampleBuffer[i] < lowPassFreq) sampleBuffer[i] = 0;
		}
	}
}
