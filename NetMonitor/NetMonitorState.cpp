#include "NetMonitorState.h"
#include "IpPacket.h"
#include <string>
#include <vector>
#include <windows.h>

#define PACKETMAXCAPACITY 30

NetMonitorState::NetMonitorState()
{
	packetsRead = 0;
}

void NetMonitorState::Initialize()
{
	NetMonitorState::UpdateNetProcs();

	NetMonitorState::lastConnectionsSort = std::chrono::system_clock::now();
	NetMonitorState::lastUIUpdate = std::chrono::system_clock::now();
	NetMonitorState::lastNetProcUpdate = std::chrono::system_clock::now();
}

void NetMonitorState::ProcessPacket(IpPacket *packet)
{
	std::string packetId = packet->pktGroupId + "-" + std::to_string(packet->pktNumber);

	// check if packet is already in 
	if (NetMonitorState::packetVersionHistory.count(packetId) != 0)
	{
		// already processed one of these 
		return;
	}

	NetMonitorState::packetVersionHistory[packetId] = packet->appearance;

	// update state with packet info
	if (packet->packetType != PacketType::Ethernet)
	{
		return;
	}

	// find if there is already a connection
	Connection * connection = NULL;

	//switch (packet->ipVersion)
	//{
	//case IPVersion::IPv4:
		connection = FindConnection(&ipConnections, packet);
	//	break;
	//case IPVersion::IPv6:
	//	connection = FindConnection(&ipv6Connections, packet);
	//	break;
	//default:
	//	// don't do anything
	//	return;		
	//}

	// if connection exists, update it
	if (connection != NULL)
	{		
		// update connection
		(*connection).UpdateConnection(packet);
	}
	else
	{
		// if not, create a new one
		Connection newConnection = Connection(packet);

		IpConnections* ipConnectionsPtr = NULL;
		
			ipConnectionsPtr = &ipConnections;

		std::vector<Connection>* connectionsPtr = NULL;
		switch (packet->transportProtocol)
		{
		case TransportProtocol::TCP:
			connectionsPtr = &((*ipConnectionsPtr).tcpConnections);
			break;
		case TransportProtocol::UDP:
			connectionsPtr = &((*ipConnectionsPtr).udpConnections);
			break;
		case TransportProtocol::ICMP:
			connectionsPtr = &((*ipConnectionsPtr).icmpConnections);
			break;
		default:
			// don't do anything
			return;
		}

		// add connection to state
		(*connectionsPtr).push_back(newConnection);
	}

	NetMonitorState::localInterfaces.insert(packet->GetLocalNetworkAddress().IpAddress);

	NetMonitorState::packetsRead++;

	// TODO: use pointer to packet instead of copy
	NetMonitorState::recentPackets.push_back(packet);
	if (recentPackets.size() == PACKETMAXCAPACITY)
	{
		delete recentPackets.front();
		recentPackets.pop_front();
	}
}

Connection* NetMonitorState::FindConnection(TransportProtocol transportProtocol, IPVersion ipVersion, NetworkAddress localNetworkAddress, NetworkAddress remoteNetworkAddress)
{
	IpConnections* ipConnections = NULL;
	
		ipConnections = &(NetMonitorState::ipConnections);


	 std::vector<Connection>* connections = NULL;
	switch (transportProtocol)
	{
	case (TransportProtocol::UDP):
		connections = &(ipConnections->udpConnections);
		break;
	case (TransportProtocol::TCP):
		connections = &(ipConnections->tcpConnections);
		break;
	default:
		return NULL;
	}

	for (int i = 0; i < connections->size(); i++)
	{
		Connection* currentConnection = &((*connections)[i]);

		if ((currentConnection->localNetworkAddress.IpAddress == localNetworkAddress.IpAddress)
			&& (currentConnection->localNetworkAddress.port == localNetworkAddress.port)

			&& (currentConnection->remoteNetworkAddress.IpAddress == remoteNetworkAddress.IpAddress)
			&& (currentConnection->remoteNetworkAddress.port == remoteNetworkAddress.port))
		{
			return currentConnection;
		}
	}

	return NULL;
}

Connection * NetMonitorState::FindConnection(IpConnections * ipConnections, IpPacket *packet)
{
	Connection * returnedConnection = NULL;

	std::vector<Connection>* connectionsPtr = NULL;

	switch (packet->transportProtocol)
	{
	case TransportProtocol::UDP:
		connectionsPtr = &((*ipConnections).udpConnections);
		break;
	case TransportProtocol::TCP:
		connectionsPtr = &((*ipConnections).tcpConnections);
		break;
	case TransportProtocol::ICMP:
		connectionsPtr = &((*ipConnections).icmpConnections);
		break;
	default:
		return NULL;
	}

	for (int i = 0; i < (*connectionsPtr).size(); i++)
	{
		Connection * connection = &((*connectionsPtr)[i]);

		if (packet->MatchNetworkAddresses((*connection).localNetworkAddress, (*connection).remoteNetworkAddress))
		{
			returnedConnection = connection;
			break;
		}
	}

	return returnedConnection;
}

bool CompareConnectionSize(Connection connection1, Connection connection2)
{
	auto compareGracePeriod = std::chrono::seconds(2);

	bool connection1IsBigger = false;

	int connection1TotalBytes = connection1.txBytes + connection1.txBytes;
	int connection2TotalBytes = connection2.txBytes + connection1.txBytes;

	auto connection1LastTrans = connection1.lastRx;
	if (connection1LastTrans < connection1.lastTx)
		connection1LastTrans = connection1.lastTx;

	auto connection2LastTrans = connection2.lastRx;
	if (connection2LastTrans < connection2.lastTx)
		connection2LastTrans = connection2.lastTx;

	if (connection1LastTrans - compareGracePeriod > connection2LastTrans)
	{
		connection1IsBigger = true;
		if (connection1TotalBytes < connection2TotalBytes)
		{
			connection1IsBigger = false;
		}
	}

	return connection1IsBigger;
}

void IpConnections::SortConnections(TransportProtocol transportProtocol)
{
	switch (transportProtocol)
	{
	case TransportProtocol::UDP:
		std::sort(udpConnections.begin(), udpConnections.end(), CompareConnectionSize);
		break;
	case TransportProtocol::TCP:
		std::sort(tcpConnections.begin(), tcpConnections.end(), CompareConnectionSize);
		break;
	case TransportProtocol::ICMP:
		std::sort(icmpConnections.begin(), icmpConnections.end(), CompareConnectionSize);
		break;
	default:
		break;
		// do not do anything;
	}
}

void IpConnections::SortAllConnections ()
{
	std::sort(udpConnections.begin(), udpConnections.end(), CompareConnectionSize);
	std::sort(tcpConnections.begin(), tcpConnections.end(), CompareConnectionSize);
	std::sort(icmpConnections.begin(), icmpConnections.end(), CompareConnectionSize);
}

void NetMonitorState::UpdateNetProcsIfNeeded()
{
	auto now = std::chrono::system_clock::now();
	auto nextUpdate = NetMonitorState::lastNetProcUpdate + +std::chrono::seconds(3);;

	if (now > nextUpdate)
	{
		NetMonitorState::UpdateNetProcs();
		NetMonitorState::lastNetProcUpdate = now;
	}
}

void NetMonitorState::UpdateNetProcs()
{
	NetMonitorState::netProcInfos.clear();
	auto netstatEntries = NetMonitorState::GetNetstat();
	auto processEntries = NetMonitorState::GetProcesses();

	NetMonitorState::netProcInfos = NetMonitorState::GetNetProcInfos(&netstatEntries, &processEntries);
}

std::vector<ProcessEntry> NetMonitorState::GetProcesses()
{
	std::vector<ProcessEntry> procEntries;
	std::string command = "C:\\Windows\\System32\\tasklist.exe";
	std::string processOutput = NetMonitorState::GetProcessOutput(command);

	auto stringTokenizer = PacketStringTokenizer(processOutput);

	std::vector<std::string> lines = stringTokenizer.GetAllTokens("\r\n");

	/* drop intro lines

	C:\>tasklist
0
1	Image Name                     PID Session Name        Session#    Mem Usage
2	========================= ======== ================ =========== ============
3	System Idle Process              0 Services                   0          8 K
4	System                           4 Services                   0      5,272 K
5	Registry                       160 Services                   0     69,428 K
*/

	int startingIndex = 3;

	for (int i = startingIndex; i < lines.size(); i++)
	{
		auto lineTokenizer = PacketStringTokenizer(lines[i]);
		std::vector<std::string> tokens = lineTokenizer.GetAllTokens(" \t");

		ProcessEntry procEntry = ProcessEntry();
		procEntry.ImageName = tokens[0];
		procEntry.PID = tokens[1];
		procEntry.SessionName = tokens[2];
		procEntry.SessionNum = tokens[3];
		procEntry.MemUsage = tokens[4];

		procEntries.push_back(procEntry);
	}

	return procEntries;
}

std::string NetMonitorState::GetProcessOutput(std::string commandString)
{
	auto cString = commandString.c_str();

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;

	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set the bInheritHandle flag so pipe handles are inherited. 
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.
	HANDLE childStdOutWrite = NULL;
	HANDLE childStdOutRead = NULL;

	if (!CreatePipe(&childStdOutRead, &childStdOutWrite, &saAttr, 0))
		return std::string();

	if (!SetHandleInformation(childStdOutRead, HANDLE_FLAG_INHERIT, 0))
		return std::string();

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(siStartInfo);
	siStartInfo.hStdError = childStdOutWrite;
	siStartInfo.hStdOutput = childStdOutWrite;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	TCHAR cmdLine[] = TEXT("C:\\Windows\\System32\\tasklist.exe");

	TCHAR* param = new TCHAR[commandString.size() + 1];
	param[commandString.size()] = 0;
	//As much as we'd love to, we can't use memcpy() because
	//sizeof(TCHAR)==sizeof(char) may not be true:
	std::copy(commandString.begin(), commandString.end(), param);

	if (!CreateProcess(
		NULL,               // application name
		param,            // command line 
		NULL,
		NULL,
		TRUE,
		0,                  // creation flags 
		NULL,            // use parent's environment 
		NULL,   // process directory
		&siStartInfo,       // STARTUPINFO pointer 
		&pi))               // receives PROCESS_INFORMATION 
	{
		int error = GetLastError();
		return std::string();
	}

	CloseHandle(childStdOutWrite);

	// plus one to add newline character
	CHAR pktmonBuffer[4096 + 1];

	DWORD bytesRead;
	BOOL readSuccess;

	std::string netstatOutput;

	while (true)
	{
		readSuccess = ReadFile(childStdOutRead, pktmonBuffer, 4096, &bytesRead, NULL);

		if (!readSuccess || bytesRead == 0)
		{
			break;
		}

		// process netstat output
		pktmonBuffer[bytesRead] = '\0';
		std::string newOutput = std::string(pktmonBuffer);

		netstatOutput += newOutput;
	}

	return netstatOutput;
}

std::vector<NetstatEntry> NetMonitorState::GetNetstat()
{
	std::vector<struct NetstatEntry> netStatEntries;

	//auto cmdLine = TEXT('netstat -ano');

	std::string cmdLine = "C:\\Windows\\System32\\NETSTAT.exe -no";

	std::string netstatOutput = NetMonitorState::GetProcessOutput(cmdLine);

	auto stringTokenizer = PacketStringTokenizer(netstatOutput);

	std::vector<std::string> lines = stringTokenizer.GetAllTokens("\r\n");

	/* drop intro lines

	C:\>netstat -noa
0
1	Active Connections
2
3	Proto  Local Address          Foreign Address        State           PID
4	TCP    0.0.0.0:135            0.0.0.0:0              LISTENING       1144
5	TCP    0.0.0.0:445            0.0.0.0:0              LISTENING       4
6   TCP    [2601:602:8d80:4f20::441f]:64639  [2606:2800:11f:17a5:191a:18d5:537:22f9]:443  ESTABLISHED     30336
7   TCP    [2601:602:8d80:4f20::441f]:64640  [2606:2800:11f:17a5:191a:18d5:537:22f9]:443  ESTABLISHED     30336
*/

	int startingIndex = 4;

	for (int i = startingIndex; i < lines.size(); i++)
	{
		auto lineTokenizer = PacketStringTokenizer(lines[i]);
		std::vector<std::string> tokens = lineTokenizer.GetAllTokens(" \t");

		NetstatEntry netStatEntry = NetstatEntry();
		netStatEntry.Proto = tokens[0];
		netStatEntry.LocalAddress = tokens[1];
		netStatEntry.ForeignAddress = tokens[2];

		if (netStatEntry.Proto == "TCP")
		{
			netStatEntry.State = tokens[3];
			netStatEntry.PID = tokens[4];
		}
		else if (netStatEntry.Proto == "UDP")
		{
			netStatEntry.PID = tokens[3];
		}

		netStatEntries.push_back(netStatEntry);
	}

	return netStatEntries;
}

ProcessEntry* NetMonitorState::FindProcess(std::vector<ProcessEntry>* processEntries, std::string PID)
{
	for (int i = 0; i < processEntries->size(); i++)
	{
		if ((*processEntries)[i].PID == PID)
		{
			return &((*processEntries)[i]);
		}
	}

	return NULL;
}

std::vector<NetProcInfo> NetMonitorState::GetNetProcInfos(std::vector<NetstatEntry>* netstatEntries, std::vector<ProcessEntry>* processEntries)
{
	std::vector<struct NetProcInfo> netProcInfos = std::vector<struct NetProcInfo>();

	for (int i = 0; i < netstatEntries->size(); i++)
	{
		NetstatEntry* netstatEntry = &((*netstatEntries)[i]);
		ProcessEntry* processEntry = NetMonitorState::FindProcess(processEntries, ((*netstatEntries)[i]).PID);

		if (processEntry == NULL)
			continue;

		struct NetProcInfo netProcInfo = NetProcInfo();
		netProcInfo.Name = processEntry->ImageName;
		netProcInfo.PID = processEntry->PID;
		netProcInfo.PROTO = netstatEntry->Proto;
		netProcInfo.State = netstatEntry->State;
		netProcInfo.LocalAddress = netstatEntry->LocalAddress;
		netProcInfo.DstAddress = netstatEntry->ForeignAddress;

		netProcInfos.push_back(netProcInfo);
	}

	return netProcInfos;
}