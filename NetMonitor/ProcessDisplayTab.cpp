#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "DnsPacket.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"


ProcessDisplayTab::ProcessDisplayTab()
{
	ProcessDisplayTab::TabName = " PROC ";

	// fourth tab;
	TabIndex = 3;
}

void ProcessDisplayTab::UpdateTabDisplay(DisplayState* display, NetMonitorState* state)
{
	WINDOW* window = display->window;
	int currentLine = display->currentLine;
	int numScreenColumns = display->numDisplayColumns;

	std::string rawHeaderline = "Name             PID    State         LocalAddr                           RemoteAddr                PROTO  Rx    Tx";

	wattron(window, A_UNDERLINE);
	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(rawHeaderline, numScreenColumns).c_str());
	wattrset(window, A_NORMAL);

	currentLine++;

	for (int i = 0; i < state->netProcInfos.size() ; i++)
	{
		NetProcInfo* netProcInfo = &((state->netProcInfos)[i]);

		std::string ipVersionString;

		if ((netProcInfo->LocalAddress).at(0) == '[')
		{
			// this is ipv6
			ipVersionString = "IPv6";
		}
		else
		{
			ipVersionString = "IPv4";
		}

		Connection* connection = NULL;

		auto ipVersion = IpPacket::ToIpVersion(ipVersionString);

		// TODO: handle empty NetworkAddress
		NetworkAddress localAddress = NetworkAddress::ParseNetstatAddressString(netProcInfo->LocalAddress, ipVersion);
		NetworkAddress remoteAddress = NetworkAddress::ParseNetstatAddressString(netProcInfo->DstAddress, ipVersion);

		auto transportProtocol = ToTransportProtocol(netProcInfo->PROTO);

		connection = state->FindConnection(transportProtocol, ipVersion, localAddress, remoteAddress);

		std::string netProcString;

		if (connection != NULL)
		{
			netProcString = ProcessDisplayTab::ToDisplayText(netProcInfo, connection);
		}
		else
		{
			netProcString = ProcessDisplayTab::ToDisplayText(netProcInfo);
		}

		if (DisplayTab::DisplayLinePassesFilter(netProcString, state->filterString))
		{
			mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(netProcString, numScreenColumns).c_str());
			currentLine++;
		}
	}

	NetMonitorDisplay::ClearScreenBelowRow(currentLine, display->numDisplayLines, display->numDisplayColumns, display->window);
}

std::string ProcessDisplayTab::ToDisplayText(NetProcInfo* netProcInfo, Connection* connection)
{
	std::string displayText = ProcessDisplayTab::ToDisplayText(netProcInfo);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection->rxBytes), 10);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection->txBytes), 10);

	return displayText;
}

std::string ProcessDisplayTab::ToDisplayText(NetProcInfo* netProcInfo)
{
	std::string displayText;

	displayText += NetMonitorDisplay::FormatLine(netProcInfo->Name, 17);
	displayText += NetMonitorDisplay::FormatLine(netProcInfo->PID, 7);
	displayText += NetMonitorDisplay::FormatLine(netProcInfo->State, 14);
	displayText += NetMonitorDisplay::FormatLine(netProcInfo->LocalAddress, 32);
	displayText += NetMonitorDisplay::FormatLine(netProcInfo->DstAddress, 32);
	displayText += NetMonitorDisplay::FormatLine(netProcInfo->PROTO, 5);

	return displayText;
}
