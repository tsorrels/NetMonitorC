#pragma once

#include <string>
#include <vector>

class PacketStringTokenizer
{
public:
	PacketStringTokenizer(std::string inputString);

	std::string GetNextToken(char delimiter);
	std::string GetNextToken(std::string delimiters);
	std::vector<std::string> GetAllTokens(std::string delimiters);

private:
	int stringPosition;
	std::string inputString;
	bool TokensLeft();
};

