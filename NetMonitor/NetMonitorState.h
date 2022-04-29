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

typedef struct NetProcInfo
{
	// 	std::string rawHeaderline = "Name   PID	     State    LocalAddress       DstAddress   PROTO    Rx    Tx";
	std::string Name;
	std::string PID;
	std::string State;
	std::string LocalAddress;
	std::string DstAddress;
	std::string PROTO;
	std::string RX;
	std::string TX;
} NetProcInfo;

typedef struct NetstatEntry
{
	std::string Proto;
	std::string LocalAddress;
	std::string ForeignAddress;
	std::string State;
	std::string PID;
} NetstatEntry;

typedef struct ProcessEntry
{
	std::string ImageName;
	std::string PID;
	std::string SessionName;
	std::string SessionNum;
	std::string MemUsage;
} ProcessEntry;

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
	void Initialize();

	IpConnections ipv4Connections;
	IpConnections ipv6Connections;
	std::map<std::string, int> packetVersionHistory;
	int packetsRead;
	std::chrono::time_point<std::chrono::system_clock> lastConnectionsSort;
	std::chrono::time_point<std::chrono::system_clock> lastUIUpdate;
	std::chrono::time_point<std::chrono::system_clock> lastNetProcUpdate;


	std::deque<IpPacket*> recentPackets;
	std::set<std::string> localInterfaces;
	std::vector<NetProcInfo> netProcInfos;

	void ProcessPacket(IpPacket* packet);
	void UpdateNetProcsIfNeeded();
	void UpdateNetProcs();
	ProcessEntry* FindProcess(std::vector<ProcessEntry>* processEntries, std::string PID);
	std::vector<NetProcInfo> GetNetProcInfos(std::vector<NetstatEntry>* netstatEntries, std::vector<ProcessEntry>* processEntries);
	std::vector<NetstatEntry> GetNetstat();
	std::vector<ProcessEntry> GetProcesses();

	Connection* FindConnection(TransportProtocol transportProtocol, IPVersion ipVersion, NetworkAddress localNetworkAddress, NetworkAddress remoteNetworkAddress);
	

private:
	Connection* FindConnection(IpConnections * ipConnections, IpPacket *packet);
	std::string GetProcessOutput(std::string commandString);
};
