#include "NetMonitorState.h"

void TcpConnection::UpdateConnection(IpPacket* packet)
{
	TcpPacket* tcpPacket = (TcpPacket*)packet;

	Connection::UpdateConnection(packet);

	if (tcpPacket->synSet)
		numSyn++;
	if (tcpPacket->finSet)
		numFin++;
	if (tcpPacket->ackSet)
		numAck++;
	if (tcpPacket->rstSet)
		numRst++;
	if (tcpPacket->pshSet)
		numPsh++;
}