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
                EFF_REVERB,
                EFF_VOICE_IN_MASK,
                EFF_PITCH_SHIFT,
        };

        static uint16_t tempBuf[10 * 1024];
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

		void Reverb(uint16_t* sampleBuffer, int samples, float decay, float density) {
			// Parameter validation and clamping (same as before)
			decay = std::max(0.0f, std::min(1.0f, decay));
			density = std::max(0.0f, std::min(1.0f, density));

			// Create a delay buffer (circular buffer for efficiency)
			static float delayBuffer[44100]; // Using float for the delay buffer to reduce quantization error accumulation
			static int writeIndex = 0;

			// Pre-calculate a decay factor per sample
			float decayPerSample = std::pow(decay, 1.0f / 44100.0f); // Assuming 44.1kHz sample rate

			for (int i = 0; i < samples; i++) {
				// Calculate the delay time in samples (e.g., 0.5 seconds delay)
				int delaySamples = static_cast<int>(44100 * 0.5f);

				// Read from the delay buffer
				int readIndex = (writeIndex - delaySamples + 44100) % 44100;
				float delayedSample = delayBuffer[readIndex];

				// Mix the original sample with the delayed sample
				float outputSample = static_cast<float>(sampleBuffer[i]) + delayedSample * density;

				// Apply decay to the delayed sample before writing it back
				delayBuffer[writeIndex] = outputSample * decayPerSample;

				// Convert the output sample to 16-bit and apply soft clipping
				outputSample = std::tanh(outputSample / 32767.0f) * 32767.0f;
				sampleBuffer[i] = static_cast<uint16_t>(outputSample);

				// Update the write index
				writeIndex = (writeIndex + 1) % 44100;
			}
		}

		void VoiceInMask(uint16_t* sampleBuffer, int samples, float resonanceFrequency, float resonanceAmount) {
			// Simple low-pass filter using a biquad filter design
			float a0 = 1.0f;
			float a1 = -1.8f; // Adjust for cutoff frequency
			float a2 = 0.81f; // Adjust for cutoff frequency
			float b1 = 0.0f;
			float b2 = 0.0f;

			// Resonance parameters
			resonanceFrequency = std::max(100.0f, std::min(800.0f, resonanceFrequency)); // Clamp to a reasonable range
			resonanceAmount = std::max(0.0f, std::min(1.0f, resonanceAmount)); // Clamp to 0-1

			// State variables for the filter
			float x1 = 0.0f, x2 = 0.0f;
			float y1 = 0.0f, y2 = 0.0f;

			for (int i = 0; i < samples; i++) {
				float x0 = static_cast<float>(sampleBuffer[i]);

				// Apply the low-pass filter
				float y0 = a0 * x0 + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;

				// Update filter state
				x2 = x1;
				x1 = x0;
				y2 = y1;
				y1 = y0;

				// Apply resonance (simplified)
				if (std::abs(y0) > resonanceFrequency) {
					y0 *= (1.0f + resonanceAmount);
				}

				// Convert back to 16-bit and apply soft clipping
				y0 = std::tanh(y0 / 32767.0f) * 32767.0f;
				sampleBuffer[i] = static_cast<uint16_t>(y0);
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