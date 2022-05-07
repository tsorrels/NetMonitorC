#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "DnsPacket.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"

#define PACKETSTODISPLAY 25

RawDisplayTab::RawDisplayTab()
{
	RawDisplayTab::TabName = " RAW ";

	// third tab;
	TabIndex = 2;
}

void RawDisplayTab::UpdateTabDisplay(DisplayState* display, NetMonitorState* state)
{
	WINDOW* window = display->window;
	int currentLine = display->currentLine;
	int numScreenColumns = display->numDisplayColumns;

	std::string rawHeaderline = "TimeStamp                 SrcIp   SrcPort              DstIp  DstPort  PROTO  Length";

	wattron(window, A_UNDERLINE);
	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(rawHeaderline, numScreenColumns).c_str());
	wattrset(window, A_NORMAL);

	currentLine++;

	//	WaitForSingleObject(state->netProcInfosMutex, INFINITE);
	WaitForSingleObject(state->recentPacketsMutex, INFINITE);

	int numPackets = state->recentPackets.size();

	for (int i = 0; i < numPackets && i < PACKETSTODISPLAY; i++)
	{
		IpPacket* packet = ((state->recentPackets)[i]);

		std::string packetString = RawDisplayTab::ToDisplayText(packet);

		if (DisplayTab::DisplayLinePassesFilter(packetString, state->filterString))
		{
			mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(packetString, numScreenColumns).c_str());
			currentLine++;
		}
	}

	ReleaseMutex(state->recentPacketsMutex);

	NetMonitorDisplay::ClearScreenBelowRow(currentLine, display->numDisplayLines, display->numDisplayColumns, display->window);
}

std::string RawDisplayTab::ToDisplayText(IpPacket* packet)
{
	std::string displayText;

	auto localNetworkAddress = packet->GetLocalNetworkAddress();
	auto remoteNetworkAddress = packet->GetRemoteNetworkAddress();

	std::string timeString = packet->timeStamp.substr(0, 11);
	displayText += NetMonitorDisplay::FormatLine(timeString, 13);
	displayText += NetMonitorDisplay::FormatLine(localNetworkAddress.IpAddress, 20, LineJustified::Right);
	displayText += "   ";
	displayText += NetMonitorDisplay::FormatLine(std::to_string(localNetworkAddress.port), 6);
	displayText += NetMonitorDisplay::FormatLine(remoteNetworkAddress.IpAddress, 20, LineJustified::Right);
	displayText += "   ";
	displayText += NetMonitorDisplay::FormatLine(std::to_string(remoteNetworkAddress.port), 6);
	displayText += NetMonitorDisplay::FormatLine(ToString(packet->transportProtocol), 5);
	displayText += "   ";
	displayText += NetMonitorDisplay::FormatLine(std::to_string(packet->length), 5);

	return displayText;
}
