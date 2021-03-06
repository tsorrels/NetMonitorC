#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "DnsPacket.h";
#include "../Dependencies/pdcurses/curses.h"

const char headerLine1[] = "netmonitor.exe 0.9.0        Press 'ESC' to exit";

enum class LineJustified
{
	Right,
	Left
};

class DisplayState
{
public:

	WINDOW* window;
	int numDisplayLines;
	int numDisplayColumns;
	int currentLine;
};

// abstract class
class DisplayTab
{
public:
	DisplayTab(int tabIndex) 
	{
		TabIndex = tabIndex;
	};

	// 0 = first tab, 1 = second tab, etc.
	int TabIndex;
	std::string TabName;
	virtual bool DisplayLinePassesFilter(std::string displayString, std::string filterString);
	virtual void UpdateTabDisplay(DisplayState* display, NetMonitorState* state) = 0;
};

class HomeDisplayTab : public DisplayTab
{
public:
	HomeDisplayTab(int tabIndex) : DisplayTab(tabIndex)
	{
		TabName = "HOME";
	};

	void UpdateTabDisplay(DisplayState* display, NetMonitorState* state) override;

private:
	void UpdateDisplayConnections(std::vector<Connection*> *connections, int linesToWrite, int numColumns, int* currentLine, WINDOW* window, std::string filterString);
	std::string ToDisplayText(Connection connection);

	std::string GetTransmitString(Connection connection);
	void ToggleBold(Connection connection, WINDOW* window);
	std::string GetRateBar(Connection connection);
};

class DnsDisplayTab : public DisplayTab
{
public:
	DnsDisplayTab(int tabIndex) : DisplayTab(tabIndex)
	{
		TabName = " DNS ";
	};

	void UpdateTabDisplay(DisplayState* display, NetMonitorState* state) override;

private:
	std::string ToDisplayText(DnsData dnsData);
};

class TcpDisplayTab : public DisplayTab
{
public:
	TcpDisplayTab(int tabIndex) : DisplayTab(tabIndex)
	{
		TabName = " TCP ";
	};

	void UpdateTabDisplay(DisplayState* display, NetMonitorState* state) override;

private:
	std::string ToDisplayText(Connection* connection);
};

class RawDisplayTab : public DisplayTab
{
public:
	RawDisplayTab(int tabIndex) : DisplayTab(tabIndex)
	{
		TabName = " RAW ";
	};

	void UpdateTabDisplay(DisplayState* display, NetMonitorState* state) override;

private:
	std::string ToDisplayText(IpPacket* packet);
};


class ProcessDisplayTab : public DisplayTab
{
public:
	ProcessDisplayTab(int tabIndex) : DisplayTab(tabIndex)
	{
		TabName = " PROC ";
	};

	void UpdateTabDisplay(DisplayState* display, NetMonitorState* state) override;

private:
	std::string ToDisplayText(NetProcInfo* netProcInfo, Connection* connection);
	std::string ToDisplayText(NetProcInfo* netProcInfo);
	std::vector<NetstatEntry> GetNetstat();
	std::vector<ProcessEntry> GetProcesses();
	ProcessEntry * FindProcess(std::vector<ProcessEntry>* processEntries, std::string PID);
	std::vector<NetProcInfo>* GetNetProcInfos(std::vector<NetstatEntry>* netstatEntries, std::vector<ProcessEntry>* processEntries);
};

class NetMonitorDisplay
{
public:

	NetMonitorState* state;
	DisplayState display;
	DisplayTab* currentTab;

	std::vector<DisplayTab*> displayTabs;
	NetMonitorDisplay(WINDOW* window, NetMonitorState* state);

	void SelectNextDisplayTab();
	void SelectPreviousDisplayTab();
	void UpdateDisplay();
	void DrawScreen();
	bool GetUserInput();
	static void ClearScreenBelowRow(int row, int numScreenRows, int numScreenColumns, WINDOW* window);

	// 	NetMonitorDisplay::ClearScreenBelowRow(currentLine, numScreenRows, numScreenColumns);

	static std::string FormatLine(std::string text, int numColumns, LineJustified justified = LineJustified::Left);
private:
	void InitializeAllDisplayTabs();
	void AddTabLinesToDisplay();
	void SetTabTextAttr(bool isPrimaryTab);
	void ClearTextAttr();

	void ProcessValidFilterInput(int input);
	void ProcessBackspace();
};
