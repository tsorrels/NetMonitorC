#include "pch.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include "../NetMonitor/PacketStringTokenizer.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NetMonitorTests
{
	TEST_CLASS(PacketStringTokenizerTests)
	{
	public:
		
		TEST_METHOD(PacketStringTokenizer_InfoString_ParseSuccessful)
		{
			// LoadNextStringIntoBuffer will remove \r\n from string
			std::string infoString = "13:11:24.300880600 PktGroupId 1970324836974593, PktNumber 1, Appearance 1, Direction Tx , Type Ethernet , Component 74, Edge 1, Filter 0, OriginalSize 164, LoggedSize 128";

			PacketStringTokenizer infoStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(infoString);

			std::vector<std::string> tokens = infoStringTokenizer.GetAllTokens(" ,");

			Assert::AreEqual(21, (int)tokens.size());
		}

		TEST_METHOD(PacketStringTokenizer_PacketString_ParseSuccessful)
		{
			// LoadNextStringIntoBuffer will remove \r\n from string
			std::string packetString = "3C-58-C2-BD-75-5E > 3C-9B-D6-E9-AB-E8, ethertype IPv4 (0x0800), length 164: 10.0.0.146.56890 > 10.0.0.5.8009: Flags [P.], seq 4207539416:4207539526, ack 3453818831, win 507, length 110";

			PacketStringTokenizer infoStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(packetString);

			std::vector<std::string> tokens = infoStringTokenizer.GetAllTokens(" ,");

			Assert::AreEqual(21, (int)tokens.size());
		}
	};
}
