#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <chrono>
#include <ctime>
#include <map>
#include <deque>
#include <set>
#include "Protocol.h"
#include "IpPacket.h"
#include "TcpPacket.h"
#include "IcmpPacket.h"
#include "UdpPacket.h"

class Connection
{
public:
	NetworkAddress localNetworkAddress;
	NetworkAddress remoteNetworkAddress;
	int txBytes;
	int rxBytes;
	std::chrono::time_point<std::chrono::system_clock> lastRx;
	std::chrono::time_point<std::chrono::system_clock> lastTx;
	TransportProtocol transportProtocol;
	Protocol::ProtoEnum protocol;
	std::string data;

	// for unit testing
	Connection()
	{
	}

	Connection(IpPacket* packet);
	void UpdateConnection(IpPacket* packet);
	std::string Serialize();

private:		
	void AddBytes(IpPacket packet);
};

class IpConnections
{
public:
	std::vector<Connection> udpConnections;
	std::vector<Connection> tcpConnections;
	std::vector<Connection> icmpConnections;

	void SortConnections(TransportProtocol transportProtocol);
	void SortAllConnections();
};

class NetMonitorState
{
public:
/*
State
|
|_ipV4Connections
|  |_udpConnections[]
|  |_tcpConnections[]
|  |_icmpConnections[]
|_ipV6Connections
   |_udpConnections[]
   |_tcpConnections[]
   |_icmp6Connections[]

*/
	NetMonitorState();

	IpConnections ipv4Connections;
	IpConnections ipv6Connections;
	std::map<std::string, int> packetVersionHistory;
	int packetsRead;

	void ProcessPacket(IpPacket *packet);
	std::deque<IpPacket*> recentPackets;
	std::set<std::string> localInterfaces;

private:
	Connection* FindConnection(IpConnections * ipConnections, IpPacket *packet);
};

