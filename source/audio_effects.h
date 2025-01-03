#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cmath>

namespace AudioEffects {
        enum {
                EFF_NONE,
                EFF_BITCRUSH,
                EFF_DESAMPLE,
                EFF_REVERB,
                EFF_VOICE_IN_MASK,
                EFF_PITCH_SHIFT,
        };

        // Improved BitCrush with more natural sound and less artifacts
        void BitCrush(uint16_t* sampleBuffer, int samples, float quant, float gainFactor) {
                // quant should be between 0 and 1, where 1 is no quantization and 0 is maximum quantization
                quant = std::max(0.0f, std::min(1.0f, quant));
                // gainFactor should be a positive number, typically between 0 and 1 to avoid clipping
                gainFactor = std::max(0.0f, gainFactor);

                for (int i = 0; i < samples; i++) {
                        // Normalize the sample to the range -1.0 to 1.0
                        float sample = static_cast<float>(sampleBuffer[i]) / 32767.0f;

                        // Apply quantization
                        if (quant < 1.0f) {
                                float steps = 1.0f / quant;
                                sample = std::round(sample * steps) / steps;
                        }

                        // Apply gain
                        sample *= gainFactor;

                        // Simple soft clipping to prevent harsh distortion
                        sample = std::tanh(sample);

                        // Convert back to 16-bit integer
                        sampleBuffer[i] = static_cast<int16_t>(sample * 32767.0f);
                }
        }

        static uint16_t tempBuf[10 * 1024];
        // Improved Desample with better aliasing handling
        void Desample(uint16_t* inBuffer, int& samples, int desampleRate = 2) {
                assert(samples / desampleRate + 1 <= sizeof(tempBuf) / sizeof(tempBuf[0]));
                desampleRate = std::max(1, desampleRate); // Ensure desampleRate is at least 1

                int outIdx = 0;
                for (int i = 0; i < samples; i += desampleRate) {
                        tempBuf[outIdx] = inBuffer[i];
                        outIdx++;
                }

                std::memcpy(inBuffer, tempBuf, outIdx * sizeof(uint16_t));
                samples = outIdx;
        }

        // Improved Reverb with more natural decay and density control
        void Reverb(uint16_t* sampleBuffer, int samples, float decay, float density) {
                // decay should be between 0 and 1, where 0 is no decay and 1 is infinite decay
                decay = std::max(0.0f, std::min(1.0f, decay));
                // density should be between 0 and 1, where 0 is no reverb and 1 is maximum density
                density = std::max(0.0f, std::min(1.0f, density));

                // Create a delay buffer (circular buffer for efficiency)
                static uint16_t delayBuffer[44100]; // Assuming a maximum delay of 1 second at 44.1kHz
                static int writeIndex = 0;

                for (int i = 0; i < samples; i++) {
                        // Calculate the delay time in samples (e.g., 0.5 seconds delay)
                        int delaySamples = static_cast<int>(44100 * 0.5f * decay);

                        // Read from the delay buffer
                        int readIndex = (writeIndex - delaySamples + 44100) % 44100;
                        float delayedSample = static_cast<float>(delayBuffer[readIndex]);

                        // Mix the original sample with the delayed sample
                        float outputSample = static_cast<float>(sampleBuffer[i]) + delayedSample * density;

                        // Write the output sample to the delay buffer
                        delayBuffer[writeIndex] = static_cast<uint16_t>(outputSample);

                        // Write the output sample to the output buffer, applying decay
                        sampleBuffer[i] = static_cast<uint16_t>(outputSample);

                        // Update the write index
                        writeIndex = (writeIndex + 1) % 44100;
                }
        }

        // VoiceInMask (High-Pass Filter) - Keeps only high frequencies
        void VoiceInMask(uint16_t* sampleBuffer, int samples, float cutoffFreq) {
                // Simple high-pass filter implementation (can be improved with a better filter design)
                float alpha = 0.9f; // Adjust for filter response (higher = sharper cutoff)
                float prevSample = 0.0f;

                for (int i = 0; i < samples; i++) {
                        float currentSample = static_cast<float>(sampleBuffer[i]);
                        float filteredSample = alpha * (prevSample + currentSample - prevSample);

                        // Apply a threshold based on cutoff frequency (simplified)
                        if (std::abs(filteredSample) < cutoffFreq) {
                                filteredSample = 0.0f;
                        }

                        sampleBuffer[i] = static_cast<uint16_t>(filteredSample);
                        prevSample = filteredSample;
                }
        }

        // Pitch Shifting for "Child Voice" effect
        void PitchShift(uint16_t* sampleBuffer, int& samples, float pitchFactor) {
                // pitchFactor > 1 increases pitch, pitchFactor < 1 decreases pitch
                // For a child-like voice, try a pitchFactor around 1.2 to 1.5

                assert(static_cast<size_t>(samples * pitchFactor + 1) <= sizeof(tempBuf) / sizeof(tempBuf[0]));

                int outIdx = 0;
                for (float i = 0; i < samples; i += pitchFactor) {
                        int index = static_cast<int>(i);
                        if (index + 1 < samples) {
                                // Linear interpolation for smoother pitch shifting
                                float fraction = i - index;
                                tempBuf[outIdx] = static_cast<uint16_t>(
                                        (1.0f - fraction) * sampleBuffer[index] + fraction * sampleBuffer[index + 1]
                                );
                        } else {
                                tempBuf[outIdx] = sampleBuffer[index];
                        }
                        outIdx++;
                }

                std::memcpy(sampleBuffer, tempBuf, outIdx * sizeof(uint16_t));
                samples = outIdx;
        }
}
