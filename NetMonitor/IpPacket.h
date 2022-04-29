#pragma once

#include "PacketStringTokenizer.h"
#include "Protocol.h"
#include <string>
#include <vector>

enum class IPVersion {IPv4 = 4, IPv6 = 6, Other = 0};

enum class Direction {Rx, Tx, Other};

enum class PacketType {Ethernet, Other};

class NetworkAddress
{
public:

	// for unit testing
	NetworkAddress() {}

	NetworkAddress(std::string IpAddress, int port)
	{
		this->IpAddress = IpAddress;
		this->port = port;
	};

	std::string IpAddress;
	int port;


	static NetworkAddress ParseNetstatAddressString(std::string networkAddressString, IPVersion ipVersion)
	{
		NetworkAddress networkAddress;

		if (networkAddressString == "*.*" || networkAddressString == "*:*")
			return networkAddress;

		// [2601:602:8d80:4f20::441f]:64639 
		// 0.0.0.0:445

		PacketStringTokenizer tokenizer = PacketStringTokenizer::PacketStringTokenizer(networkAddressString);

		std::vector<std::string> ipTokens;

		switch (ipVersion)
		{
		case (IPVersion::IPv4):
			ipTokens = tokenizer.GetAllTokens(":");
			if (ipTokens.size() == 2)
			{
				// TODO: handle *.* address
				networkAddress.IpAddress = ipTokens[0];
				networkAddress.port = stoi(ipTokens[1]);
			}

			break;
			
		case (IPVersion::IPv6):		
			ipTokens = tokenizer.GetAllTokens("[]");

			if (ipTokens.size() == 2)
			{
				networkAddress.IpAddress = ipTokens[0];

				std::string portString = ipTokens[1].substr(1, ipTokens[1].length());
				networkAddress.port = stoi(portString);
			}

			break;
					
		}

		return networkAddress;
	}
};

// TODO: make IpPacket abstract class
class IpPacket
{
public:
	IpPacket(PacketType type)
	{
		type = type;
	}

	virtual std::string GetData()
	{
		return std::string("");
	}

	virtual NetworkAddress GetLocalNetworkAddress()
	{
		NetworkAddress networkAddress = NetworkAddress();
		networkAddress.IpAddress = this->localIpAddress;
		networkAddress.port = 0;

		return networkAddress;
	}

	virtual NetworkAddress GetRemoteNetworkAddress()
	{
		NetworkAddress networkAddress = NetworkAddress();
		networkAddress.IpAddress = this->remoteIpAddress;
		networkAddress.port = 0;

		return networkAddress;
	}

	virtual bool MatchNetworkAddresses(NetworkAddress networkAddress1, NetworkAddress networkAddress2)
	{
		if ((this->localIpAddress == networkAddress1.IpAddress && this->remoteIpAddress == networkAddress2.IpAddress) ||
			(this->localIpAddress == networkAddress2.IpAddress && this->remoteIpAddress == networkAddress1.IpAddress))
		{
			return true;
		}

		return false;
	}

	IpPacket(std::vector<std::string> infoTokens, std::vector<std::string> ipTokens)
	{
		timeStamp = infoTokens[0];
		pktGroupId = infoTokens[2];
		pktNumber = std::stoi(infoTokens[4]);
		appearance = std::stoi(infoTokens[6]);
		direction = ToDirection(infoTokens[8]);
		packetType = ToPacketType(infoTokens[10]);

		std::string ethertype = ipTokens[3];
		ipVersion = ToIpVersion(ipTokens[4]);

		// remove tailing ':'
		length = std::stoi(ipTokens[7].substr(0, ipTokens[7].length() - 1));

		switch (direction)
		{
		case Direction::Tx:
			localIpAddress = GetIpFromNetworkAddress(ipTokens[8], ipVersion);
			remoteIpAddress = GetIpFromNetworkAddress(ipTokens[10], ipVersion);		
			break;
		case Direction::Rx:
		default:
			localIpAddress = GetIpFromNetworkAddress(ipTokens[10], ipVersion);
			remoteIpAddress = GetIpFromNetworkAddress(ipTokens[8], ipVersion);
		}
	}

	std::string Serialize()
	{
		std::string serializedString;

		serializedString += (timeStamp + "\t");
		serializedString += (localIpAddress + "\t");
		serializedString += (remoteIpAddress + "\t");
		serializedString += (std::to_string(length) + "\t");

		return serializedString;
	}

	Direction static ToDirection(std::string directionString)
	{
		Direction direction = Direction::Other;

		if (directionString == "Tx")
			direction = Direction::Tx;
		else if (directionString == "Rx")
			direction = Direction::Rx;

		return direction;
	}

	IPVersion static ToIpVersion(std::string ipVersionString)
	{
		IPVersion ipVersion = IPVersion::Other;

		if (ipVersionString == "IPv4")
			ipVersion = IPVersion::IPv4;
		else if (ipVersionString == "IPv6")
			ipVersion = IPVersion::IPv6;

		return ipVersion;
	}

	PacketType static ToPacketType(std::string packetTypeString)
	{
		PacketType packetType = PacketType::Other;

		if (packetTypeString == "Ethernet")
			packetType = PacketType::Ethernet;
		
		return packetType;
	}

	std::string static GetIpFromNetworkAddress(std::string networkAddress, IPVersion ipVersion)
	{
		std::string IpAddress = networkAddress;
		PacketStringTokenizer tokenizer = PacketStringTokenizer::PacketStringTokenizer(networkAddress);
		std::vector<std::string> tokens = tokenizer.GetAllTokens(".");

		switch (ipVersion)
		{
		case IPVersion::IPv4:
			if (tokens.size() == 5)
			{
				int position = networkAddress.find_last_of('.', networkAddress.length());
				if (position == std::string::npos)
				{
					// handle this
				}

				IpAddress = networkAddress.substr(0, position);
			}

			break;

		case IPVersion::IPv6:
			if (tokens.size() == 2)
				IpAddress = tokens[0];

		default:
			break;
		}

		int ipLength = IpAddress.length();

		if (IpAddress.at(ipLength - 1) == ':')
		{
			IpAddress = IpAddress.substr(0, ipLength - 1);
		}

		return IpAddress;
	}

	std::string static GetPortFromNetworkAddress(std::string networkAddress, IPVersion ipVersion)
	{
		std::string port;
		PacketStringTokenizer tokenizer = PacketStringTokenizer::PacketStringTokenizer(networkAddress);
		std::vector<std::string> tokens = tokenizer.GetAllTokens(".");

		switch (ipVersion)
		{
		case IPVersion::IPv4:
			if (tokens.size() == 5)
				port = tokens[4];

			break;

		case IPVersion::IPv6:
			if (tokens.size() == 2)
				port = tokens[1];

		default:
			break;
		}

		return port;
	}

	std::string timeStamp;

	std::string pktGroupId;

	int pktNumber;

	int appearance;

	std::string localIpAddress;

	std::string remoteIpAddress;

	PacketType packetType;

	int length;

	IPVersion ipVersion;		// IPv4 || IPv6

	Direction direction;		// rx || tx

	TransportProtocol transportProtocol;
};
