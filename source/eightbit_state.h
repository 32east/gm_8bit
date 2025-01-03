#pragma once
#include <string>
#include <unordered_map>

struct EightbitState {
	int crushFactor = 350;
	float gainFactor = 1.2;
	bool broadcastPackets = false;
	int desampleRate = 2;
	float decay = 0.5;
	int delay = 1000;
	float mufflingFactor = 0.7;
	float factor = 0.0;
	uint16_t port = 4000;
	std::string ip = "127.0.0.1";
	std::unordered_map<int, std::tuple<IVoiceCodec*, int>> afflictedPlayers;
};
