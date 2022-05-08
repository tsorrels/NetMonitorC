#include "NetMonitorState.h"

Connection * ConnectionFactory::CreateConnection(IpPacket* packet)
{
	Connection* newConnection = NULL;

	auto transportProtocol = packet->transportProtocol;

	switch (transportProtocol)
	{
	case TransportProtocol::TCP:
		newConnection = new TcpConnection();

		// TODO: refactor transportProtocol field to something cleaner on the Connection/TcpConnection object
		newConnection->transportProtocol = transportProtocol;
		break;

	case TransportProtocol::UDP:
	case TransportProtocol::ICMP:
		newConnection = new Connection();
		newConnection->transportProtocol = transportProtocol;
		break;

	case TransportProtocol::Other:
	default:
		return NULL;
	}


	newConnection->ipVersion = packet->ipVersion;

	newConnection->localNetworkAddress = packet->GetLocalNetworkAddress();
	newConnection->remoteNetworkAddress = packet->GetRemoteNetworkAddress();

	newConnection->txBytes = 0;
	newConnection->rxBytes = 0;
	newConnection->lastTx = std::chrono::system_clock::now();
	newConnection->lastRx = std::chrono::system_clock::now();
	newConnection->rxRate = 0;
	newConnection->txRate = 0;

	newConnection->UpdateBytes(*packet);

	auto localProto = Protocol::GetProtocol(transportProtocol, newConnection->localNetworkAddress.port);
	auto remoteProto = Protocol::GetProtocol(transportProtocol, newConnection->remoteNetworkAddress.port);

	if (localProto != Protocol::ProtoEnum::Other)
	{
		newConnection->protocol = localProto;
	}
	else if (remoteProto != Protocol::ProtoEnum::Other)
	{
		newConnection->protocol = remoteProto;
	}
	else
	{
		newConnection->protocol = Protocol::ProtoEnum::Other;
	}

	return newConnection;
}