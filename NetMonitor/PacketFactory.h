#pragma once

#include <vector>
#include "IpPacket.h"

class PacketFactory
{
public:
	static IpPacket * CreatePacket(std::vector<std::string> tokens, std::vector<std::string> packetTokens);
};
