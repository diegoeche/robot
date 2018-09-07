#pragma once
#include <thread>

// Loop-requesting Stats WIP
void StatsServer();

// Sends a command using UDP
void sendCommand(const char message[]);
void networkInitialize(const char server[]);
