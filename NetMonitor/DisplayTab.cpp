#include "NetMonitorDisplay.h";

bool DisplayTab::DisplayLinePassesFilter(std::string displayString, std::string filterString)
{
	auto location = displayString.find(filterString);

	if (location != std::string::npos)
		return true;

	return false;
}