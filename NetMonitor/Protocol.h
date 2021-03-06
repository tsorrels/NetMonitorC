#pragma once

#include <string>
#include <map>
#include "NetMonitorEnums.h"



static TransportProtocol ToTransportProtocol(std::string transportProtocolString)
{
	if (transportProtocolString == "TCP")
		return TransportProtocol::TCP;

	if (transportProtocolString == "UDP")
		return TransportProtocol::UDP;

	if (transportProtocolString == "ICMP")
		return TransportProtocol::ICMP;

	return TransportProtocol::Other;
}

static std::string ToString(TransportProtocol proto)
{
	std::string protoString;

	switch (proto)
	{
	case TransportProtocol::UDP:
		protoString = "UDP";
		break;
	case TransportProtocol::TCP:
		protoString = "TCP";
		break;
	case TransportProtocol::ICMP:
		protoString = "ICMP";
		break;
	case TransportProtocol::Other:
	default:
		protoString = "Other";
	}

	return protoString;
}

class Protocol
{
public:
	enum class ProtoEnum
	{
		HTTPS,
		NetBIOS,
		DNS,
		SMB,
		SSDP,
		QUIC,
		RPC,
		RDP,
		MSSQL,
		WINRM,
		Other
	};

	static ProtoEnum GetProtocol(TransportProtocol transportProtocol, int port)
	{
		ProtoEnum proto = ProtoEnum::Other;

		std::map<int, ProtoEnum>::iterator iter;

		switch (transportProtocol)
		{
		case TransportProtocol::TCP:
			iter = tcpProtocolMap.find(port);
			if (iter != tcpProtocolMap.end())
				proto = iter->second;

			break;

		case TransportProtocol::UDP:
			iter = udpProtocolMap.find(port);
			if (iter != udpProtocolMap.end())
				proto = iter->second;
	
			break;

		case TransportProtocol::Other:
		default:
			break;
		}

		return proto;
	}

	static std::string ToString(Protocol::ProtoEnum protocol)
	{
		switch (protocol)
		{
		case Protocol::ProtoEnum::DNS:
			return "DNS";
		case Protocol::ProtoEnum::HTTPS:
			return "HTTPS";
		case Protocol::ProtoEnum::SMB:
			return "SMB";
		case Protocol::ProtoEnum::QUIC:
			return "QUIC";
		case Protocol::ProtoEnum::RPC:
			return "RPC";
		case Protocol::ProtoEnum::RDP:
			return "RDP";
		case Protocol::ProtoEnum::NetBIOS:
			return "NetBIOS";
		case Protocol::ProtoEnum::SSDP:
			return "SSDP";
		case Protocol::ProtoEnum::MSSQL:
			return "MSSQL";
		case Protocol::ProtoEnum::WINRM:
			return "WINRM";

		default:
			break;
		}

		return "Other";
	}

private:
	static std::map<int, Protocol::ProtoEnum> tcpProtocolMap;
	static std::map<int, Protocol::ProtoEnum> udpProtocolMap;
};