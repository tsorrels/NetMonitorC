#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "DnsPacket.h";
#include "NetMonitorDisplay.h";
#include "NetMonitorEnums.h";
#include "../Dependencies/pdcurses/curses.h"

const int MaxConnectionsToDisplay = 100;

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
	std::string columnHeadersString = "IPVer  RemoteIp                             Rx       Tx PROTO SrcPort DstPort PROTO Kb/s";
	mvwaddstr(window, currentLine, 0, columnHeadersString.c_str());
	wattrset(window, A_NORMAL);
	currentLine++;

	// (*state).ipConnections.SortAllConnections();

	WaitForSingleObject(state->connectionsProcInfosMutex, INFINITE);
	UpdateDisplayConnections((*state).ipConnections.allConnections, MaxConnectionsToDisplay, numScreenColumns, &currentLine, display->window);

	NetMonitorDisplay::ClearScreenBelowRow(currentLine, display->numDisplayLines, display->numDisplayColumns, display->window);

	ReleaseMutex(state->connectionsProcInfosMutex);
}

std::string HomeDisplayTab::ToDisplayText(Connection connection)
{
	std::string displayText = " ";

	std::string ipVersion = EnumConverter::ToString(connection.ipVersion);
	displayText += NetMonitorDisplay::FormatLine(ipVersion, 6); 
	
	//displayText += GetTransmitString(connection);

	displayText += NetMonitorDisplay::FormatLine(connection.remoteNetworkAddress.IpAddress, 30);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.rxBytes), 9, LineJustified::Right);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.txBytes), 9, LineJustified::Right);

	displayText += " ";

	std::string transportProtocol = EnumConverter::ToString(connection.transportProtocol);
	displayText += NetMonitorDisplay::FormatLine(transportProtocol, 6);

	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.localNetworkAddress.port), 6);
	displayText += "  ";
	displayText += NetMonitorDisplay::FormatLine(std::to_string(connection.remoteNetworkAddress.port), 6);
	displayText += "  ";
	displayText += NetMonitorDisplay::FormatLine(Protocol::ToString(connection.protocol), 5);

	displayText += "  ";

	bool isTransmitting = false;
	auto maxTimeStamp = (std::max)(connection.lastRx, connection.lastTx);
	if (maxTimeStamp > std::chrono::system_clock::now() - std::chrono::seconds(2))
		isTransmitting = true;

	if (isTransmitting)
	{
		double maxRate = (std::max)(connection.rxRate, connection.txRate);
		displayText += NetMonitorDisplay::FormatLine(std::to_string((int)maxRate), 5);
	}

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
	for (int i = 0; i < linesToWrite && i < connections.size(); i++)
	{
		// turn on bold

		Connection* connection = &(connections[i]);

		std::string displayLine = HomeDisplayTab::ToDisplayText(*connection);

		HomeDisplayTab::ToggleBold(*connection, window);
		mvwaddstr(window, *currentLine, 0, NetMonitorDisplay::FormatLine(displayLine, numColumns).c_str());

		// add STANDOUT blocks to line
		if (connection->IsActive()) 
		{
			wattron(window, A_STANDOUT);
			int startingCol = displayLine.length() + 1;
			std::string rateBar = GetRateBar(*connection);
			mvwaddstr(window, *currentLine, startingCol, rateBar.c_str());
		}

		(*currentLine)++;
		wattrset(window, 0);
	}
}

std::string HomeDisplayTab::GetRateBar(Connection connection)
{
	double maxRate = (std::max)(connection.rxRate, connection.txRate);
	std::string spaces = " ";
	int numSpaces = maxRate / 100;

	for (int i = 0; i < numSpaces; i++)
	{
		spaces += " ";
	}

	return spaces;
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
