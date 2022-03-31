#include "NetMonitorState.h"
#include "IpPacket.h"
#include <string>
#include <vector>

#define PACKETMAXCAPACITY 30

NetMonitorState::NetMonitorState()
{
	packetsRead = 0;
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

	switch (packet->ipVersion)
	{
	case IPVersion::IPv4:
		connection = FindConnection(&ipv4Connections, packet);
		break;
	case IPVersion::IPv6:
		connection = FindConnection(&ipv6Connections, packet);
		break;
	default:
		// don't do anything
		return;		
	}

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
		
		switch (packet->ipVersion)
		{
		case IPVersion::IPv4:
			ipConnectionsPtr = &ipv4Connections;
			break;
		case IPVersion::IPv6:
			ipConnectionsPtr = &ipv6Connections;
			break;
		default:
			// don't do anything
			return;
		}

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

		std::vector<Connection>* ipv4connectionsptr = &(ipv4Connections.tcpConnections);

		// add connection to state
		(*connectionsPtr).push_back(newConnection);
	}

	NetMonitorState::localInterfaces.insert(packet->GetLocalNetworkAddress().IpAddress);

	NetMonitorState::packetsRead++;

	// NetMonitorState::recentPackets.resize(98);

	// TODO: use pointer to packet instead of copy
	NetMonitorState::recentPackets.push_back(packet);
	if (recentPackets.size() == PACKETMAXCAPACITY)
	{
		delete recentPackets.front();
		recentPackets.pop_front();
	}
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
