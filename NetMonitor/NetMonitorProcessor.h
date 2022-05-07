#pragma once
#include "PacketProcessor.h"
#include "NetMonitorState.h"

class NetMonitorProcessor
{
public:
	NetMonitorProcessor(PacketProcessor* packetProcessor, NetMonitorState* state);
	void Run();

private:
	PacketProcessor* packetProcessor;
	NetMonitorState* state;
};

