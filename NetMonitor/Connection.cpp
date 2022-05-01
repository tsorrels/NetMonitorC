#include "NetMonitorState.h"

Connection::Connection(IpPacket* packet)
{
	ipVersion = packet->ipVersion;

	localNetworkAddress = packet->GetLocalNetworkAddress();
	remoteNetworkAddress = packet->GetRemoteNetworkAddress();

	txBytes = 0;
	rxBytes = 0;
	lastTx = std::chrono::system_clock::now() - std::chrono::hours(24);
	lastRx = std::chrono::system_clock::now() - std::chrono::hours(24);

	AddBytes(*packet);

	transportProtocol = packet->transportProtocol;
	data = packet->GetData();
	auto localProto = Protocol::GetProtocol(transportProtocol, localNetworkAddress.port);
	auto remoteProto = Protocol::GetProtocol(transportProtocol, remoteNetworkAddress.port);

	if (localProto != Protocol::ProtoEnum::Other)
	{
		protocol = localProto;
	}
	else if (remoteProto != Protocol::ProtoEnum::Other)
	{
		protocol = remoteProto;
	}
	else
	{
		protocol = Protocol::ProtoEnum::Other;
	}
}

void Connection::UpdateConnection(IpPacket* packet)
{
	AddBytes(*packet);

	// TODO: do something with data
	data += packet->GetData();
}

std::string Connection::Serialize()
{
	// remoteIp
	// TODO: add local ip address
	return "    " + remoteNetworkAddress.IpAddress + "    " + std::to_string(txBytes) + "    " + std::to_string(rxBytes) + "    " + std::to_string((int)transportProtocol);
}

void Connection::AddBytes(IpPacket packet)
{
	switch (packet.direction)
	{
	case Direction::Rx:
		rxBytes += packet.length;
		lastRx = std::chrono::system_clock::now();
		break;
	case Direction::Tx:
		txBytes += packet.length;
		lastTx = std::chrono::system_clock::now();
		break;
	default:
		break;
	}
}