#pragma once
#include <string>
#include <unordered_map>

struct EightbitState {
	int crushFactor = 350;
	float gainFactor = 1.2;
	bool broadcastPackets = false;
	int desampleRate = 2;
	float decay = 0.5;
	float reverbDensity = 0.5;
	float resonanceFrequency = 500;
	float resonanceAmount = 0.5;
	float pitchFactor = 1.2;
	uint16_t port = 4000;
	std::string ip = "127.0.0.1";
	std::unordered_map<int, std::tuple<IVoiceCodec*, int>> afflictedPlayers;
};
