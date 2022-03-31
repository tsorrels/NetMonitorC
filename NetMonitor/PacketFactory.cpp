#include "PacketFactory.h"
#include <algorithm>
#include <vector>
#include "PacketProcessor.h"
#include "PacketStringTokenizer.h"

#include "DnsPacket.h"
#include "IcmpPacket.h"
#include "UdpPacket.h"
#include "TcpPacket.h"
#include "IpPacket.h"

IpPacket * PacketFactory::CreatePacket(std::vector<std::string> infoTokens, std::vector<std::string> packetTokens)
{
    // jump ahead to protocol string
    IpPacket * packetPtr = NULL; // (PacketType::Other);

    if (infoTokens.size() < 11 || packetTokens.size() < 10)
    {
        // packetPtr.packetType = PacketType::Other;
        return NULL;
    }

    if (infoTokens[10] != "Ethernet")
    {
        //packetPtr.packetType = PacketType::Other;
        return NULL;
    }

    if (packetTokens[4] != "IPv4" && packetTokens[4] != "IPv6")
    {
        //packetPtr.packetType = PacketType::Other;
        return NULL;
    }

    if (packetTokens[8] == "bad-len")
    {
        //packetPtr.packetType = PacketType::Other;
        return NULL;
    }

    std::string protocalString = packetTokens[11];
    std::vector<std::string> ipTokens(&packetTokens[0], &packetTokens[11]);
    std::vector<std::string> protoTokens(&packetTokens[11], &packetTokens[packetTokens.size() - 1]);

    // packet.type = Ethernet;
        // parse protocolString
    if (protocalString == "ICMP" || protocalString == "ICMP6")
    {
        packetPtr = new IcmpPacket(infoTokens, ipTokens, protoTokens);
    }
    else if (protocalString == "UDP")
    {
        packetPtr = new UdpPacket(infoTokens, ipTokens, protoTokens);
    }
    else if (protocalString == "Flags")
    {
        packetPtr = new TcpPacket(infoTokens, ipTokens, protoTokens);
    }
    else if (protocalString.at(protocalString.length() - 1) == '+')
    {
        packetPtr = new DnsPacket(infoTokens, ipTokens, protoTokens);

        // TODO: do not hack this to a NULL pointer, handle all packet types
        if (packetPtr->packetType == PacketType::Other)
        {
            packetPtr = NULL;
        }
    }
    else
    {
        //packetPtr.packetType = PacketType::Other;
        return NULL;
    }

	return packetPtr;
}