#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>

namespace AudioEffects {
    enum EffectType {
        EFF_NONE,
        EFF_BITCRUSH,
        EFF_DESAMPLE,
        EFF_VOICE_IN_MASK,
        EFF_REVERB,
        EFF_PITCH,
        EFF_COMBINE
    };

    static uint16_t tempBuf[10 * 1024];

    void BitCrush(uint16_t* sampleBuffer, int samples, int bitDepth = 4) {
        uint16_t step = 1 << (16 - bitDepth);
        for (int i = 0; i < samples; i++) {
            sampleBuffer[i] = (sampleBuffer[i] / step) * step;
        }
    }

    void Desample(uint16_t* sampleBuffer, int& samples, int desampleRate = 2) {
        assert(samples / desampleRate + 1 <= sizeof(tempBuf) / sizeof(tempBuf[0]));
        int outIdx = 0;
        for (int i = 0; i < samples; i++) {
            if (i % desampleRate == 0) continue;
            tempBuf[outIdx++] = sampleBuffer[i];
        }
        std::memcpy(sampleBuffer, tempBuf, outIdx * sizeof(uint16_t));
        samples = outIdx;
    }

    void LowPassFilter(uint16_t* sampleBuffer, int samples, float alpha = 0.1f) {
        float prev = static_cast<float>(sampleBuffer[0]);
        for (int i = 1; i < samples; i++) {
            float current = static_cast<float>(sampleBuffer[i]);
            prev = alpha * current + (1 - alpha) * prev;
            sampleBuffer[i] = static_cast<uint16_t>(prev);
        }
    }

    void VoiceInMask(uint16_t* sampleBuffer, int samples) {
        LowPassFilter(sampleBuffer, samples, 0.25f);
        for (int i = 0; i < samples; i++) {
            sampleBuffer[i] = static_cast<uint16_t>(sampleBuffer[i] * (0.8f + 0.2f * std::sin(i * 0.05f)));
        }
    }

    void Reverb(uint16_t* sampleBuffer, int samples, float decay = 0.5f, int delay = 441) {
        for (int i = samples - 1; i >= delay; i--) {
            sampleBuffer[i] += static_cast<uint16_t>(decay * sampleBuffer[i - delay]);
        }
    }

    void PitchShift(uint16_t* sampleBuffer, int samples, float factor) {
        int outSamples = static_cast<int>(samples / factor);
        for (int i = 0; i < outSamples; i++) {
            int index = static_cast<int>(i * factor);
            sampleBuffer[i] = sampleBuffer[index % samples];
        }
        std::fill(sampleBuffer + outSamples, sampleBuffer + samples, 0);
    }

    void Combine(uint16_t* sampleBuffer, int samples) {
        VoiceInMask(sampleBuffer, samples);
        Reverb(sampleBuffer, samples, 0.3f);
        PitchShift(sampleBuffer, samples, 0.85f);
    }
}
