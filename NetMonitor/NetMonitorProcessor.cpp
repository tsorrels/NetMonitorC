#include "NetMonitorProcessor.h"
#include "NetMonitorState.h"
#include "PacketProcessor.h"

NetMonitorProcessor::NetMonitorProcessor(PacketProcessor* packetProcessor, NetMonitorState* state)
{
	NetMonitorProcessor::packetProcessor = packetProcessor;
	NetMonitorProcessor::state = state;
}

void NetMonitorProcessor::Run()
{
	while (true)
	{
		IpPacket* packet = packetProcessor->GetNextPacket();

		state->ProcessPacket(packet);
		state->UpdateNetProcsIfNeeded();
	}
}