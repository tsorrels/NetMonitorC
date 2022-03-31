

#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "DnsPacket.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"



DnsDisplayTab::DnsDisplayTab()
{
	DnsDisplayTab::TabName = " DNS ";

	// second tab;
	TabIndex = 1;
}


void DnsDisplayTab::UpdateTabDisplay(DisplayState* display, NetMonitorState* state)
{
	WINDOW* window = display->window;
	int currentLine = display->currentLine;
	int numScreenColumns = display->numDisplayColumns;

	wattron(window, A_UNDERLINE);
	std::string dnsHeaderline = " DNSServerIp         RequestId      RecordType     DomainName";
	mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(dnsHeaderline, numScreenColumns).c_str());
	wattrset(window, A_NORMAL);

	currentLine++;

	//(*state).ipv4Connections.SortAllConnections();
	//(*state).ipv6Connections.SortAllConnections();

	std::vector<DnsData> dnsDatas;
	dnsDatas.reserve(200);

	for (int i = 0; i < state->ipv4Connections.udpConnections.size(); i++)
	{
		Connection* connection = &(state->ipv4Connections.udpConnections[i]);
		if (connection->protocol != Protocol::ProtoEnum::DNS)
			continue;

		std::vector<DnsData> dnsDataObjects = DnsData::FromMultipleDataString(connection->data);
		dnsDatas.insert(dnsDatas.end(), dnsDataObjects.begin(), dnsDataObjects.end());
	}

	for (int i = 0; i < state->ipv6Connections.udpConnections.size(); i++)
	{
		Connection* connection = &(state->ipv6Connections.udpConnections[i]);
		if (connection->protocol != Protocol::ProtoEnum::DNS)
			continue;

		std::vector<DnsData> dnsDataObjects = DnsData::FromMultipleDataString(connection->data);
		dnsDatas.insert(dnsDatas.end(), dnsDataObjects.begin(), dnsDataObjects.end());
	}

	std::sort(dnsDatas.begin(), dnsDatas.end(), DnsData::CompareDnsData);

	for (int j = 0; j < dnsDatas.size(); j++)
	{
		auto dnsDataObject = dnsDatas[j];
		std::string dnsConnectionLine = ToDisplayText(dnsDataObject);
		mvwaddstr(window, currentLine, 0, NetMonitorDisplay::FormatLine(dnsConnectionLine, numScreenColumns).c_str());
		currentLine++;
	}

	NetMonitorDisplay::ClearScreenBelowRow(currentLine, display->numDisplayLines, display->numDisplayColumns, display->window);
}

std::string DnsDisplayTab::ToDisplayText(DnsData dnsData)
{
	std::string displayText;

	displayText += "    ";
	displayText += NetMonitorDisplay::FormatLine(dnsData.resolverAddress, 25);
	displayText += NetMonitorDisplay::FormatLine(std::to_string(dnsData.requestId), 10);
	displayText += NetMonitorDisplay::FormatLine(DnsData::ToRecordTypeString(dnsData.recordType), 10);
	displayText += NetMonitorDisplay::FormatLine(dnsData.domainName, 30);

	return displayText;
}