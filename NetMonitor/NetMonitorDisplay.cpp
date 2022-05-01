#include <string>
#include <vector>
#include <chrono>
#include "NetMonitorState.h";
#include "DnsPacket.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"

using namespace std::literals;

void NetMonitorDisplay::InitializeAllDisplayTabs()
{
	HomeDisplayTab* homeDisplayTab = new HomeDisplayTab();
	DnsDisplayTab* dnsDisplayTab = new DnsDisplayTab();
	RawDisplayTab* rawDisplayTab = new RawDisplayTab();
	ProcessDisplayTab* processDisplayTab = new ProcessDisplayTab();

	// by convention the HOME tab is always first
	NetMonitorDisplay::displayTabs.push_back(homeDisplayTab);
	NetMonitorDisplay::displayTabs.push_back(dnsDisplayTab);
	NetMonitorDisplay::displayTabs.push_back(rawDisplayTab);
	NetMonitorDisplay::displayTabs.push_back(processDisplayTab);
}

bool NetMonitorDisplay::GetUserInput()
{
	bool terminate = false;

	int input = wgetch(NetMonitorDisplay::display.window);

	int currentTabIndex = NetMonitorDisplay::currentTab->TabIndex;

	switch (input)
	{
	case -1:
		// no input to handle
		break;

	case KEY_LEFT:
		if (currentTabIndex != 0)
			NetMonitorDisplay::currentTab = NetMonitorDisplay::displayTabs[currentTabIndex - 1];

		break;

	case KEY_RIGHT:
		if (currentTabIndex < NetMonitorDisplay::displayTabs.size() - 1)
			NetMonitorDisplay::currentTab = NetMonitorDisplay::displayTabs[currentTabIndex + 1];

		break;

	// KEY_BACKSPACE
	case '\b':		
		NetMonitorDisplay::ProcessBackspace();
		break;

	// KEY_ESC
	case 27: 
		terminate = true;
		break;

	// TODO: handle term_resize input
	default:
		char inputChar = input;

		if (input >= 0 && input <= 255 && isprint(input))
		{
			ProcessValidFilterInput(input);
		}

		break;
	}

	return terminate;
}

void NetMonitorDisplay::ProcessBackspace()
{
	if (NetMonitorDisplay::state->filterString.size() > 0)
	{
		std::string currentString = NetMonitorDisplay::state->filterString;
		auto currentStringLength = currentString.length();
		NetMonitorDisplay::state->filterString = currentString.substr(0, currentStringLength - 1);
	}
}

void NetMonitorDisplay::ProcessValidFilterInput(int input)
{
	NetMonitorDisplay::state->filterString += input;
}

NetMonitorDisplay::NetMonitorDisplay(WINDOW* window, NetMonitorState* state)
{
	//DisplayState
	NetMonitorDisplay::display.window = window;
	NetMonitorDisplay::state = state;

	mvwaddstr(NetMonitorDisplay::display.window, 0, 0, headerLine1);
	mvwaddstr(NetMonitorDisplay::display.window, 1, 0, headerLine2);

	InitializeAllDisplayTabs();

	NetMonitorDisplay::currentTab = NetMonitorDisplay::displayTabs[0];
}

void NetMonitorDisplay::DrawScreen()
{
	// redrawwin(NetMonitorDisplay::window);
	wrefresh(NetMonitorDisplay::display.window);
}

void NetMonitorDisplay::SetTabTextAttr(bool isPrimaryTab)
{
	if (isPrimaryTab)
		wattrset(NetMonitorDisplay::display.window, A_STANDOUT);
	else
		wattrset(NetMonitorDisplay::display.window, A_UNDERLINE);
}

void NetMonitorDisplay::ClearTextAttr()
{
	wattrset(NetMonitorDisplay::display.window, A_NORMAL);
}

void NetMonitorDisplay::AddTabLinesToDisplay()
{
	// display tab headings
	std::string tabTops = " ";

	// current column
	int curCol = 0;

	mvwaddstr(NetMonitorDisplay::display.window, 5, curCol, NetMonitorDisplay::FormatLine("_", NetMonitorDisplay::display.numDisplayColumns).c_str());
	curCol++;

	for (int i = 0; i < NetMonitorDisplay::displayTabs.size(); i++)
	{
		auto tabPointer = displayTabs[i];

		mvwaddstr(NetMonitorDisplay::display.window, 5, curCol, "/");
		tabTops += " ";
		curCol++;

		bool isPrimaryTab = (NetMonitorDisplay::currentTab == tabPointer);
		SetTabTextAttr(isPrimaryTab);
		// write

		mvwaddstr(NetMonitorDisplay::display.window, 5, curCol, tabPointer->TabName.c_str());
		for (int j = 0; j < tabPointer->TabName.length(); j++)
		{
			tabTops += "_";
		}

		curCol += tabPointer->TabName.length();

		ClearTextAttr();

		mvwaddstr(NetMonitorDisplay::display.window, 5, curCol, "\\");
		tabTops += " ";
		curCol++;
	}

	std::string underscores;

	int k = curCol;
	while (k < NetMonitorDisplay::display.numDisplayColumns)
	{
		underscores += '_';
		k++;
	}

	mvwaddstr(NetMonitorDisplay::display.window, 5, curCol, underscores.c_str());
	mvwaddstr(NetMonitorDisplay::display.window, 4, 0, tabTops.c_str());
}

void NetMonitorDisplay::UpdateDisplay()
{
	resize_term(0, 0);

	NetMonitorDisplay::display.numDisplayLines = LINES;
	NetMonitorDisplay::display.numDisplayColumns = COLS;

	std::string filterString = "FILTER:" + state->filterString;
	std::string packetsReadString = "Packets read: " + std::to_string(NetMonitorDisplay::state->packetsRead);
	packetsReadString += "          ";
	packetsReadString += filterString;

	mvwaddstr(NetMonitorDisplay::display.window, 3, 0, NetMonitorDisplay::FormatLine(packetsReadString, NetMonitorDisplay::display.numDisplayColumns).c_str());
	
	AddTabLinesToDisplay();

	auto currentDisplay = NetMonitorDisplay::currentTab;

	NetMonitorDisplay::display.currentLine = 7;

	currentDisplay->UpdateTabDisplay(&(NetMonitorDisplay::display), NetMonitorDisplay::state);
}

void NetMonitorDisplay::ClearScreenBelowRow(int row, int numScreenRows, int numScreenColumns, WINDOW* window)
{
	std::string blankLine = "";

	for (int i = 0; i < numScreenColumns; i++)
		blankLine += " ";

	for (int j = row; j < numScreenColumns; j++)
	{
		mvwaddstr(window, j, 0, blankLine.c_str());
	}
}

 std::string NetMonitorDisplay::FormatLine(std::string text, int numColumns, LineJustified justified)
{
	std::string formattedString;

	// crop text
	if (text.length() > numColumns)
	{
		formattedString = text.substr(0, numColumns);
	}
	// pad text
	else if (text.length() < numColumns)
	{
		formattedString = text;
		int sizePadding = numColumns - text.length();
		for (int i = 0; i < sizePadding; i++)
		{
			switch (justified)
			{
			case LineJustified::Left:
				formattedString += " ";
				break;
			case LineJustified::Right:
			default:
				formattedString = " " + formattedString;
			}
		}
	}
	else
	{
		formattedString = text;
	}

	return formattedString;
}