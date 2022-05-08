#include "NetMonitorProcessor.h"
#include "NetMonitorState.h"
#include "PacketProcessor.h"

NetMonitorProcessor::NetMonitorProcessor(PacketProcessor* packetProcessor, NetMonitorState* state, bool* continueFlag)
{
	NetMonitorProcessor::packetProcessor = packetProcessor;
	NetMonitorProcessor::state = state;
	NetMonitorProcessor::continueFlag = continueFlag;
}

void NetMonitorProcessor::Run()
{
	while (true)
	{
		IpPacket* packet = packetProcessor->GetNextPacket();

		state->ProcessPacket(packet);
		state->UpdateNetProcsIfNeeded();

		if (!(*continueFlag))
			break;
	}
}