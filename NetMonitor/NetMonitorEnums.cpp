#include "NetMonitorEnums.h"

std::string EnumConverter::ToString(IPVersion ipVersion)
{
	switch (ipVersion)
	{
	case IPVersion::IPv4:
		return "IPv4";
	case IPVersion::IPv6:
		return "IPv6";
	case IPVersion::Other:
	default:
		return "Other";
	}
}

std::string EnumConverter::ToString(Direction direction)
{
	switch (direction)
	{
	case Direction::Rx:
		return "Rx";
	case Direction::Tx:
		return "Tx";
	case Direction::Other:
	default:
		return "Other";
	}
}

std::string EnumConverter::ToString(PacketType packetType)
{
	switch (packetType)
	{
	case PacketType::Ethernet:
		return "Ethernet";
	case PacketType::Other:
	default:
		return "Other";
	}
}

std::string EnumConverter::ToString(TransportProtocol transportProtocol)
{
	switch (transportProtocol)
	{
	case TransportProtocol::TCP:
		return "TCP";
	case TransportProtocol::UDP:
		return "UDP";
	case TransportProtocol::ICMP:
		return "ICMP";
	case TransportProtocol::Other:
	default:
		return "Other";
	}
}

template<class T>
T EnumConverter::FromString(T, std::string inputString)
{
	if (T == Direction)
	{
		return Direction::Other;
	}
}