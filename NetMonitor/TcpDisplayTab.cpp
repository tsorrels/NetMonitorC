#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "TcpPacket.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"

TcpDisplayTab::TcpDisplayTab()
{
	TcpDisplayTab::TabName = " TCP ";

	// fourth tab;
	TabIndex = 4;
}

void TcpDisplayTab::UpdateTabDisplay(DisplayState* display, NetMonitorState* state)
{
	WINDOW* window = display->window;
	int currentLine = display->currentLine;
	int numScreenColumns = display->numDisplayColumns;

	wattron(window, A_UNDERLINE);
	std::string columnHeadersString = "IPVer  RemoteIp                           Pkts      Rx       Tx SrcPort DstPort  PROTO   SYN  ACK  FIN  PSH  RST";
	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(columnHeadersString, numScreenColumns).c_str());
	wattrset(window, A_NORMAL);

	currentLine++;

	for (int i = 0; i < state->ipConnections.allConnections.size(); i++)
	{
		Connection* connection = (state->ipConnections.allConnections[i]);
		if (connection->transportProtocol != TransportProtocol::TCP)
			continue;

		std::string displayLine = ToDisplayText(connection);

		if (DisplayTab::DisplayLinePassesFilter(displayLine, state->filterString))
		{
			mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(displayLine, numScreenColumns).c_str());
			currentLine++;
		}
	}

	NetMonitorDisplay::ClearScreenBelowRow(currentLine, display->numDisplayLines, display->numDisplayColumns, display->window);
}

std::string TcpDisplayTab::ToDisplayText(Connection* connection)
{
	// 	std::string columnHeadersString = "IPVer  RemoteIp                         Pkts     Rx       Tx      SrcPort DstPort    PROTO   SYN   ACK   FIN   PSH   RST";

	TcpConnection* tcpConnection = (TcpConnection*)connection;

	std::string displayText = " ";

	std::string ipVersion = EnumConverter::ToString(tcpConnection->ipVersion);
	displayText += NetMonitorDisplay::FormatLine(ipVersion, 6);

	//displayText += GetTransmitString(connection);

	displayText += NetMonitorDisplay::FormatLine(tcpConnection->remoteNetworkAddress.IpAddress, 29);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->numPackets), 9, LineJustified::Right);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->rxBytes), 9, LineJustified::Right);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->txBytes), 9, LineJustified::Right);

	displayText += " ";

	//std::string transportProtocol = EnumConverter::ToString(connection.transportProtocol);
	//displayText += NetMonitorDisplay::FormatLine(transportProtocol, 6);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->localNetworkAddress.port), 6);
	displayText += "  ";
	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->remoteNetworkAddress.port), 6);
	displayText += "   ";
	displayText += NetMonitorDisplay::FormatLine(Protocol::ToString(tcpConnection->protocol), 5);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->numSyn), 5, LineJustified::Right);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->numAck), 5, LineJustified::Right);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->numFin), 5, LineJustified::Right);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->numPsh), 5, LineJustified::Right);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(tcpConnection->numRst), 5, LineJustified::Right);

	return displayText;
}