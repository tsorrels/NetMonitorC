#pragma once

#include "IpPacket.h"

class UdpPacket : public IpPacket
{

public:

	// for unit testing
	UdpPacket() : IpPacket(PacketType::Ethernet)
	{

	}

	UdpPacket(std::vector<std::string> infoTokens, std::vector<std::string> ipTokens, std::vector<std::string> protoTokens) : IpPacket(infoTokens, ipTokens)
	{
		transportProtocol = TransportProtocol::UDP;

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
		// TODO: made cleaner

		bool ipMatch = false;

		if ((this->localIpAddress == networkAddress1.IpAddress && this->remoteIpAddress == networkAddress2.IpAddress) ||
			(this->localIpAddress == networkAddress2.IpAddress && this->remoteIpAddress == networkAddress1.IpAddress))
		{
			ipMatch = true;
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
};

