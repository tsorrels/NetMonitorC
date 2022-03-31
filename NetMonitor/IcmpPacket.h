#pragma once

#include "IpPacket.h"
#include <string>

class IcmpPacket : public IpPacket
{
public: 

	IcmpPacket(std::vector<std::string> infoTokens, std::vector<std::string> ipTokens, std::vector<std::string> protoTokens) : IpPacket(infoTokens, ipTokens)
	{
		// TODO: maybe put this down in the IP packet constructor.  IP Packets do know their higher level protocol.
		transportProtocol = TransportProtocol::ICMP;
	}
};
