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
#include <Windows.h>
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
	double rxRate;
	double txRate;
	std::chrono::time_point<std::chrono::system_clock> lastRx;
	std::chrono::time_point<std::chrono::system_clock> lastTx;
	TransportProtocol transportProtocol;
	Protocol::ProtoEnum protocol;
	std::string data;
	IPVersion ipVersion;
	int numPackets;

	// for unit testing
	Connection()
	{
	}

	virtual void UpdateConnection(IpPacket* packet);
	std::string Serialize();
	double Getkbs(std::chrono::time_point<std::chrono::system_clock> lastRx, int packetLength);
	bool IsActive();
	void UpdateBytes(IpPacket packet);
};

class ConnectionFactory
{
public:
	Connection* CreateConnection(IpPacket* packet);
};

class TcpConnection : public Connection
{
public:
	int numRst;
	int numFin;
	int numAck;
	int numSyn;
	int numPsh;

	void UpdateConnection(IpPacket* packet);
};

class IpConnections
{
public:
	std::vector<Connection*> allConnections;

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
	void Initialize();

	// Not Used
	std::set<std::string> localInterfaces;

	HANDLE connectionsProcInfosMutex;
	IpConnections ipConnections;
	ConnectionFactory connectionFactory;
	std::map<std::string, int> packetVersionHistory;

	int packetsRead;
	std::string filterString;

	std::chrono::time_point<std::chrono::system_clock> lastConnectionsSort;
	std::chrono::time_point<std::chrono::system_clock> lastUIUpdate;
	std::chrono::time_point<std::chrono::system_clock> lastNetProcUpdate;

	HANDLE recentPacketsMutex;
	std::deque<IpPacket*> recentPackets;

	HANDLE netProcInfosMutex;
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
	Connection* FindConnection(IpPacket *packet);
	std::string GetProcessOutput(std::string commandString);
};

