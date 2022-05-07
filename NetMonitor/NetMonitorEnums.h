#pragma once

#include<string>

enum class IPVersion { IPv4, IPv6, Other};

enum class Direction { Rx, Tx, Other };

enum class PacketType { Ethernet, Other };

enum class TransportProtocol { UDP, TCP, ICMP, Other };

class EnumConverter
{

public:
	static std::string ToString(IPVersion ipVersion);
	static std::string ToString(Direction direction);
	static std::string ToString(PacketType direction);
	static std::string ToString(TransportProtocol direction);

	template<class T> static
	T FromString(T, std::string inputString);
	//static Direction FromString(std::string inputString);
	//static PacketType FromString(std::string inputString);
	//static TransportProtocol FromString(std::string inputString);

};

