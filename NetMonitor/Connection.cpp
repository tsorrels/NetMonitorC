#include "NetMonitorState.h"

Connection::Connection(IpPacket* packet)
{
	ipVersion = packet->ipVersion;

	localNetworkAddress = packet->GetLocalNetworkAddress();
	remoteNetworkAddress = packet->GetRemoteNetworkAddress();

	txBytes = 0;
	rxBytes = 0;
	lastTx = std::chrono::system_clock::now();
	lastRx = std::chrono::system_clock::now();
	rxRate = 0;
	txRate = 0;

	UpdateBytes(*packet);

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
	UpdateBytes(*packet);

	numPackets++;

	// TODO: do something with data
	data += packet->GetData();
}

std::string Connection::Serialize()
{
	// remoteIp
	// TODO: add local ip address
	return "    " + remoteNetworkAddress.IpAddress + "    " + std::to_string(txBytes) + "    " + std::to_string(rxBytes) + "    " + std::to_string((int)transportProtocol);
}

void Connection::UpdateBytes(IpPacket packet)
{
	double currentKbs = 0;
	double newKbs = 0;

	switch (packet.direction)
	{
	case Direction::Rx:
		currentKbs = Connection::Getkbs(lastRx, packet.length);
		newKbs = currentKbs + rxRate / 2;		
		rxRate = newKbs;
		rxBytes += packet.length;
		lastRx = std::chrono::system_clock::now();
		break;

	case Direction::Tx:
		currentKbs = Connection::Getkbs(lastTx, packet.length);
		newKbs = currentKbs + txRate / 2;
		txRate = newKbs;
		txBytes += packet.length;
		lastTx = std::chrono::system_clock::now();
		break;

	default:
		break;
	}
}

double Connection::Getkbs(std::chrono::time_point<std::chrono::system_clock> lastRx, int packetLength)
{
	double currentMbs = 0;
	auto now = std::chrono::system_clock::now();

	auto timeDelta = now - lastRx;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeDelta).count();
	if (milliseconds == 0)
		milliseconds++;

	double seconds = (double)milliseconds/1000;
	double bps = (double)packetLength * 8 / seconds;
	double Kbs = bps / 1000;
	currentMbs = Kbs / 1000;

	return Kbs;
}

bool Connection::IsActive()
{
	bool isActive = false;

	auto maxTimeStamp = (std::max)(lastRx, lastTx);

	if (maxTimeStamp > (std::chrono::system_clock::now() - std::chrono::seconds(1)))
		isActive = true;

	return isActive;
}