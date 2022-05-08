#pragma once
#include "PacketProcessor.h"
#include "NetMonitorState.h"

class NetMonitorProcessor
{
public:
	NetMonitorProcessor(PacketProcessor* packetProcessor, NetMonitorState* state, bool* continueFlag);
	void Run();
	bool * continueFlag;

private:
	PacketProcessor* packetProcessor;
	NetMonitorState* state;
};

