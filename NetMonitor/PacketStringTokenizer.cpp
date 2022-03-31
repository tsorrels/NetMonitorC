#include "PacketStringTokenizer.h"

PacketStringTokenizer::PacketStringTokenizer(std::string inputString)
{
	PacketStringTokenizer::inputString = inputString;
	PacketStringTokenizer::stringPosition = 0;
}

std::vector<std::string> PacketStringTokenizer::GetAllTokens(std::string delimiters)
{
	std::vector<std::string> tokens;

	while (true)
	{
		std::string token = PacketStringTokenizer::GetNextToken(delimiters);

		if (token.empty())
		{
			break;
		}

		tokens.push_back(token);
	}

	return tokens;
}

std::string PacketStringTokenizer::GetNextToken(std::string delimiters)
{
	std::string token;

	if (PacketStringTokenizer::stringPosition > inputString.size())
	{
		return "";
	}

	int position = inputString.find_first_of(delimiters, PacketStringTokenizer::stringPosition);
	if (position == std::string::npos)
	{
		// end of string, advance pointer to end of string to return what is left
		position = inputString.length();
	}

	// token will include delimeter
	int tokenLength = position - PacketStringTokenizer::stringPosition;

	// get token, but leave off delimeter
	token = inputString.substr(PacketStringTokenizer::stringPosition, tokenLength);

	// advance pointer and eat delimiter
	PacketStringTokenizer::stringPosition = position + 1;

	// if the token is a delimiter, skip it
	if (token == " " || token == "," || token == "")
	{
		token = PacketStringTokenizer::GetNextToken(delimiters);
	}

	return token;
}

std::string PacketStringTokenizer::GetNextToken(char delimiter)
{
	std::string token;
	int position = inputString.find(delimiter, PacketStringTokenizer::stringPosition);
	if (position == std::string::npos)
	{
		return token;
	}

	// get token
	token = inputString.substr(PacketStringTokenizer::stringPosition, position);

	// advance pointer and eat delimiter
	PacketStringTokenizer::stringPosition = position + 1;

	return token;
}

bool PacketStringTokenizer::TokensLeft()
{
	if (stringPosition < inputString.length())
	{
		return true;
	}

	return false;
}