#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>

namespace AudioEffects {
        enum {
                EFF_NONE,
                EFF_BITCRUSH,
                EFF_DESAMPLE,
                EFF_REVERB,
                EFF_VOICE_IN_MASK
        };

        void BitCrush(int16_t* sampleBuffer, int samples, float quant, float gainFactor) {
                for (int i = 0; i < samples; i++) {
                        // Quantization (Bit Depth Reduction)
                        float quantizedValue = std::round(sampleBuffer[i] / quant) * quant;

                        // Apply gain and clamp to prevent clipping
                        int16_t result = static_cast<int16_t>(quantizedValue * gainFactor);
                        result = std::max(std::min(result, (int16_t)32767), (int16_t)-32768);

                        sampleBuffer[i] = result;
                }
        }

        static std::vector<int16_t> tempBuf; // Use a vector for dynamic allocation
        void Desample(int16_t* inBuffer, int& samples, int desampleRate = 2) {
                if (desampleRate <= 1) return; // No desampling needed

                tempBuf.resize((samples + desampleRate -1 ) / desampleRate); // Allocate enough space

                int outIdx = 0;
                for (int i = 0; i < samples; i += desampleRate) {
                        tempBuf[outIdx] = inBuffer[i];
                        outIdx++;
                }
                std::memcpy(inBuffer, tempBuf.data(), outIdx * sizeof(int16_t));
                samples = outIdx;
        }

        void Reverb(int16_t* sampleBuffer, int samples, float decay, float density) {
                std::vector<int16_t> delayBuffer(samples); // Delay buffer

                for (int i = 0; i < samples; i++) {
                        // Calculate the delayed sample index
                        int delaySamples = static_cast<int>(i * decay); 

                        if (delaySamples < samples) {
                                // Mix the original sample with the delayed sample
                                int16_t delayedSample = (delaySamples > 0) ? delayBuffer[delaySamples-1] : 0;
                                sampleBuffer[i] = std::max(std::min((int16_t)(sampleBuffer[i] + delayedSample * density), (int16_t)32767), (int16_t)-32768);
                        }
                        
                        // Store the current sample in the delay buffer
                        delayBuffer[i] = sampleBuffer[i];
                }
        }

        void VoiceInMask(int16_t* sampleBuffer, int samples, float lowPassFreq) {
                // Simple low-pass filter (not a very good one, but serves the purpose)
                // This is a very basic implementation and can be improved significantly
                // with a proper filter design (e.g., Butterworth, Chebyshev).
                
                // For simplicity, we'll use a single-pole low-pass filter.
                float alpha = 0.1f; // Adjust this value for different cutoff frequencies (higher alpha = lower cutoff)
                
                if (lowPassFreq > 0)
                {
                        int16_t y_prev = 0;
                        for (int i = 0; i < samples; i++) {
                                y_prev = static_cast<int16_t>(alpha * sampleBuffer[i] + (1.0f - alpha) * y_prev);
                                if (std::abs(sampleBuffer[i]) > lowPassFreq)
                                {
                                        sampleBuffer[i] = y_prev;
                                }
                        }
                }
                else
                {
                        for (int i = 0; i < samples; i++)
                        {
                                if (std::abs(sampleBuffer[i]) > -lowPassFreq)
                                {
                                        sampleBuffer[i] = 0;
                                }
                        }
                }
        }
}
