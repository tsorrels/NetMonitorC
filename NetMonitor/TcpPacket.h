#pragma once

#include "IpPacket.h"
#include "Protocol.h"
#include <vector>

class TcpPacket : public IpPacket
{
public:

	// For unit testing
	TcpPacket():IpPacket(PacketType::Other)
	{

	}

	TcpPacket(std::vector<std::string> infoTokens, std::vector<std::string> ipTokens, std::vector<std::string> protoTokens) : IpPacket(infoTokens, ipTokens)
	{
		// TODO: maybe put this down in the IP packet constructor.  IP Packets do know their higher level protocol.
		transportProtocol = TransportProtocol::TCP;

		switch (direction)
		{
		case Direction::Tx:
			localPort = std::stoi(GetPortFromNetworkAddress(ipTokens[8], ipVersion));
			remotePort = std::stoi(GetPortFromNetworkAddress(ipTokens[10], ipVersion));
			break;
		case Direction::Rx:
		default:
			localPort = std::stoi(GetPortFromNetworkAddress(ipTokens[10], ipVersion));
			remotePort = std::stoi(GetPortFromNetworkAddress(ipTokens[8], ipVersion));
		}

		synSet = false; ackSet = false; finSet = false; rstSet = false; pshSet = false;

		std::string flags = protoTokens[1];

		if (flags.find("S") != std::string::npos)
			synSet = true;
		if (flags.find(".") != std::string::npos)
			ackSet = true;
		if (flags.find("F") != std::string::npos)
			finSet = true;
		if (flags.find("R") != std::string::npos)
			rstSet = true;
		if (flags.find("P") != std::string::npos)
			pshSet = true;
	};

	virtual NetworkAddress GetLocalNetworkAddress()
	{
		NetworkAddress networkAddress = NetworkAddress();
		networkAddress.IpAddress = this->localIpAddress;
		networkAddress.port = localPort;

		return networkAddress;
	}

	virtual NetworkAddress GetRemoteNetworkAddress()
	{
		NetworkAddress networkAddress = NetworkAddress();
		networkAddress.IpAddress = this->remoteIpAddress;
		networkAddress.port = remotePort;

		return networkAddress;
	}

	bool MatchNetworkAddresses(NetworkAddress networkAddress1, NetworkAddress networkAddress2)
	{
		// TODO: refactor out to PacketUtilities class
		// TODO: made cleaner

		bool ipMatch = false;

		if ((this->localIpAddress == networkAddress1.IpAddress && this->remoteIpAddress == networkAddress2.IpAddress) ||
			(this->localIpAddress == networkAddress2.IpAddress && this->remoteIpAddress == networkAddress1.IpAddress))
		{
			ipMatch  = true;
		}

		if (!ipMatch)
			return false;

		if ((this->localPort == networkAddress1.port && this->remotePort == networkAddress2.port) ||
			(this->localPort == networkAddress2.port && this->remotePort == networkAddress1.port))
		{
			return true;
		}

		return false;
	}

	int localPort;

	int remotePort;

	bool synSet;

	bool ackSet;

	bool finSet;

	bool pshSet;

	bool rstSet;
};
