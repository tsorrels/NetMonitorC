#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "DnsPacket.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"


const int MaxConnectionsToDisplay = 5;

HomeDisplayTab::HomeDisplayTab()
{
	HomeDisplayTab::TabName = "HOME";

	// first tab;
	TabIndex = 0;
}

void HomeDisplayTab::UpdateTabDisplay(DisplayState* display, NetMonitorState* state)
{
	// main display

	WINDOW* window = display->window;
	int currentLine = display->currentLine;
	int numScreenColumns = display->numDisplayColumns;

	// Column headers
	wattron(window, A_UNDERLINE);
	std::string columnHeadersString = "LocalInf      RemoteIp               Rx        Tx     SrcPort  DstPort  PROTO";
	mvwaddstr(window, currentLine, 0, columnHeadersString.c_str());
	wattrset(window, A_NORMAL);
	currentLine++;

	(*state).ipv4Connections.SortAllConnections();
	(*state).ipv6Connections.SortAllConnections();

	// ipv4 connections 
	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("IPV4", numScreenColumns).c_str());
	currentLine++;

	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("  UDP", numScreenColumns).c_str());
	currentLine++;
	UpdateDisplayConnections((*state).ipv4Connections.udpConnections, MaxConnectionsToDisplay, numScreenColumns, &currentLine, display->window);

	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("  TCP", numScreenColumns).c_str());
	currentLine++;
	UpdateDisplayConnections((*state).ipv4Connections.tcpConnections, MaxConnectionsToDisplay, numScreenColumns, &currentLine, display->window);

	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("  ICMP", numScreenColumns).c_str());
	currentLine++;
	UpdateDisplayConnections((*state).ipv4Connections.icmpConnections, MaxConnectionsToDisplay, numScreenColumns, &currentLine, display->window);

	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("IPV6", numScreenColumns).c_str());
	currentLine++;

	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("  UDP", numScreenColumns).c_str());
	currentLine++;
	UpdateDisplayConnections((*state).ipv6Connections.udpConnections, MaxConnectionsToDisplay, numScreenColumns, &currentLine, display->window);

	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("  TCP", numScreenColumns).c_str());
	currentLine++;
	UpdateDisplayConnections((*state).ipv6Connections.tcpConnections, MaxConnectionsToDisplay, numScreenColumns, &currentLine, display->window);

	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine("  ICMP", numScreenColumns).c_str());
	currentLine++;
	HomeDisplayTab::UpdateDisplayConnections((*state).ipv6Connections.icmpConnections, MaxConnectionsToDisplay, numScreenColumns, &currentLine, display->window);

	NetMonitorDisplay::ClearScreenBelowRow(currentLine, display->numDisplayLines, display->numDisplayColumns, display->window);
}

std::string HomeDisplayTab::ToDisplayText(Connection connection)
{
	// 		return "    " + remoteNetworkAddress.IpAddress + "    " + std::to_string(txBytes) + "    " + std::to_string(rxBytes) + "    " + std::to_string((int)transportProtocol);
	std::string displayText;

	displayText += "    ";
	displayText += GetTransmitString(connection);
	displayText += NetMonitorDisplay::FormatLine(connection.remoteNetworkAddress.IpAddress, 30);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.rxBytes), 9);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.txBytes), 9);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.localNetworkAddress.port), 6);
	displayText += "  ";
	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.remoteNetworkAddress.port), 6);
	displayText += "  ";
	displayText += NetMonitorDisplay::FormatLine(Protocol::ToString(connection.protocol), 5);

	return displayText;
}

void HomeDisplayTab::ToggleBold(Connection connection, WINDOW* window)
{
	auto maxTimeStamp = connection.lastRx;
	if (maxTimeStamp < connection.lastTx)
	{
		maxTimeStamp = connection.lastTx;
	}

	auto boldTarget = std::chrono::system_clock::now() - std::chrono::seconds(2);
	auto dimTarget = std::chrono::system_clock::now() - std::chrono::seconds(5);

	if (maxTimeStamp > boldTarget)
	{
		wattron(window, A_BOLD);
	}
	else if (maxTimeStamp < dimTarget)
	{
		wattron(window, A_DIM);
	}
}

void HomeDisplayTab::UpdateDisplayConnections(std::vector<Connection> connections, int linesToWrite, int numColumns, int* currentLine, WINDOW* window)
{
	for (int i = 0; i < 5 && i < connections.size(); i++)
	{
		// turn on bold

		Connection* connection = &(connections[i]);



		std::string displayLine = HomeDisplayTab::ToDisplayText(*connection);

		//if (DisplayTab::DisplayLinePassesFilter(displayLine, state->filterString))
		//{



			HomeDisplayTab::ToggleBold(*connection, window);
			mvwaddstr(window, *currentLine, 0, NetMonitorDisplay::FormatLine(displayLine, numColumns).c_str());
			(*currentLine)++;

		//}

		wattrset(window, 0);
		// int wattrset(WINDOW *win, chtype attrs);
	}
}

std::string HomeDisplayTab::GetTransmitString(Connection connection)
{
	bool rx = false;
	bool tx = false;

	std::string transmitString;

	auto target = std::chrono::system_clock::now() - std::chrono::milliseconds(500);

	if (connection.lastRx > target)
	{
		rx = true;
	}

	if (connection.lastTx > target)
	{
		tx = true;
	}

	if (rx && tx)
	{
		transmitString = "<->";
	}
	else if (rx)
	{
		transmitString = "<- ";
	}
	else if (tx)
	{
		transmitString = " ->";
	}
	else
	{
		transmitString = "   ";
	}

	return transmitString;
}
