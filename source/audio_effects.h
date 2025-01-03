#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cmath>

namespace AudioEffects {
	enum EffectType {
		EFF_NONE,
		EFF_BITCRUSH,
		EFF_VOICE_IN_MASK,
		EFF_REVERB,
		EFF_PITCH,
		EFF_COMBINE
	};

	void BitCrush(uint16_t* sampleBuffer, int samples, float quant, float gainFactor) {
		for (int i = 0; i < samples; i++) {
			float f = static_cast<float>(sampleBuffer[i]);
			f /= quant;
			sampleBuffer[i] = static_cast<uint16_t>(f);
			sampleBuffer[i] *= quant;
			sampleBuffer[i] *= gainFactor;
		}
	}

	void VoiceInMask(uint16_t* sampleBuffer, int samples, float mufflingFactor = 0.7f) {
		for (int i = 0; i < samples; i++) {
			sampleBuffer[i] = static_cast<uint16_t>(sampleBuffer[i] * mufflingFactor);
		}
	}

	void Reverb(uint16_t* sampleBuffer, int samples, float decay = 0.5f, int delay = 1000) {
		for (int i = delay; i < samples; i++) {
			float reverbedSample = sampleBuffer[i] + decay * sampleBuffer[i - delay];
			sampleBuffer[i] = static_cast<uint16_t>(std::min(32767.0f, std::max(-32768.0f, reverbedSample)));
		}
	}

	void PitchShift(uint16_t* sampleBuffer, int samples, float factor) {
		for (int i = 0; i < samples; i++) {
			float shifted = sampleBuffer[i] * factor;
			sampleBuffer[i] = static_cast<uint16_t>(std::min(32767.0f, std::max(-32768.0f, shifted)));
		}
	}

	void Combine(uint16_t* sampleBuffer, int samples) {
		VoiceInMask(sampleBuffer, samples, 0.6f);  // Simulate mask-like sound
		Reverb(sampleBuffer, samples, 0.4f, 800);  // Add reverb effect
	}
}
