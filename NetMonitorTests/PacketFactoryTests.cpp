#include "pch.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include <../NetMonitor/DnsPacket.h>
#include <../NetMonitor/TcpPacket.h>
#include <../NetMonitor/UdpPacket.h>
#include <../NetMonitor/IcmpPacket.h>
#include "../NetMonitor/PacketFactory.h"
#include "../NetMonitor/PacketStringTokenizer.h"

// #include "PacketFactory.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NetMonitorTests
{
	TEST_CLASS(PacketFactoryTests)
	{
	public:

		TEST_METHOD(PacketFactory_CreatePacket_TcpIpv4)
		{
			std::string infoString = "13:11:24.300880600 PktGroupId 1970324836974593, PktNumber 1, Appearance 1, Direction Rx , Type Ethernet , Component 74, Edge 1, Filter 0, OriginalSize 164, LoggedSize 128";
			std::string packetString = "3C-58-C2-BD-75-5E > 3C-9B-D6-E9-AB-E8, ethertype IPv4 (0x0800), length 164: 10.0.0.146.56890 > 10.0.0.5.8009: Flags [P.], seq 4207539416:4207539526, ack 3453818831, win 507, length 110";

			PacketStringTokenizer infoStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(infoString);
			std::vector<std::string> infoTokens= infoStringTokenizer.GetAllTokens(" ,");

			PacketStringTokenizer packetStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(packetString);
			std::vector<std::string> packetTokens = packetStringTokenizer.GetAllTokens(" ,");

			IpPacket * packet = PacketFactory::CreatePacket(infoTokens, packetTokens);			

			NetworkAddress localNetworkAddress = packet->GetLocalNetworkAddress();
			NetworkAddress remoteNetworkAddress = packet->GetRemoteNetworkAddress();

			Assert::AreEqual(std::string("10.0.0.5"), localNetworkAddress.IpAddress);
			Assert::AreEqual(8009, localNetworkAddress.port);
			Assert::AreEqual(std::string("10.0.0.146"), remoteNetworkAddress.IpAddress);
			Assert::AreEqual(56890, remoteNetworkAddress.port);
			Assert::AreEqual((int)TransportProtocol::TCP, (int)packet->transportProtocol);

			delete packet;
		}

		TEST_METHOD(PacketFactory_CreatePacket_TcpIpv6)
		{
			std::string infoString = "17:43:21.054211000 PktGroupId 244, PktNumber 1, Appearance 6, Direction Tx , Type Ethernet , Component 27, Edge 1, Filter 0, OriginalSize 75, LoggedSize 75";
			std::string packetString = "3C-58-C2-BD-75-5E > 48-F7-C0-BA-59-91, ethertype IPv6 (0x86dd), length 75: 2601:602:8d80:4f20:f44a:2fc6:a253:d582.49176 > 2001:558:feed:443::13.443: Flags [.], seq 2185752404:2185752405, ack 2391236098, win 515, length 1";

			PacketStringTokenizer infoStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(infoString);
			std::vector<std::string> infoTokens = infoStringTokenizer.GetAllTokens(" ,");

			PacketStringTokenizer packetStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(packetString);
			std::vector<std::string> packetTokens = packetStringTokenizer.GetAllTokens(" ,");

			IpPacket *packet = PacketFactory::CreatePacket(infoTokens, packetTokens);

			NetworkAddress localNetworkAddress = packet->GetLocalNetworkAddress();
			NetworkAddress remoteNetworkAddress = packet->GetRemoteNetworkAddress();

			Assert::AreEqual(std::string("2601:602:8d80:4f20:f44a:2fc6:a253:d582"), localNetworkAddress.IpAddress);
			Assert::AreEqual(49176, localNetworkAddress.port);

			Assert::AreEqual(std::string("2001:558:feed:443::13"), remoteNetworkAddress.IpAddress);
			Assert::AreEqual(443, remoteNetworkAddress.port);

			delete packet;
		}

		TEST_METHOD(PacketFactory_CreatePacket_Icmpv6)
		{
			std::string infoString = "17:43:20.868514200 PktGroupId 1970324836974765, PktNumber 1, Appearance 9, Direction Rx , Type Ethernet , Component 26, Edge 1, Filter 0, OriginalSize 174, LoggedSize 128 ";
			std::string packetString = "48-F7-C0-BA-59-91 > 33-33-00-00-00-01, ethertype IPv6 (0x86dd), length 174: fe80::4af7:c0ff:feba:5991 > ff02::1: ICMP6, router advertisement, length 120";

			PacketStringTokenizer infoStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(infoString);
			std::vector<std::string> infoTokens = infoStringTokenizer.GetAllTokens(" ,");

			PacketStringTokenizer packetStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(packetString);
			std::vector<std::string> packetTokens = packetStringTokenizer.GetAllTokens(" ,");

			IpPacket * packet = PacketFactory::CreatePacket(infoTokens, packetTokens);

			NetworkAddress localNetworkAddress = packet->GetLocalNetworkAddress();

			Assert::AreEqual(std::string("ff02::1"), localNetworkAddress.IpAddress);
			Assert::AreEqual(0, localNetworkAddress.port);
			Assert::AreEqual(174, packet->length);

			delete packet;
		}
	

	TEST_METHOD(PacketFactory_CreatePacket_Dns)
	{
		std::string infoString = "16:30:39.009537400 PktGroupId 1970324836978912, PktNumber 1, Appearance 2, Direction Tx , Type Ethernet , Component 492, Edge 1, Filter 1, OriginalSize 92, LoggedSize 92 ";
		std::string packetString = "3C-58-C2-BD-75-5E > 48-F7-C0-BA-59-91, ethertype IPv6 (0x86dd), length 92: 2601:602:8d80:4f20::441f.55259 > 2001:558:feed::1.53: 58668+ A? www.bing.com. (30)";

		PacketStringTokenizer infoStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(infoString);
		std::vector<std::string> infoTokens = infoStringTokenizer.GetAllTokens(" ,");

		PacketStringTokenizer packetStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(packetString);
		std::vector<std::string> packetTokens = packetStringTokenizer.GetAllTokens(" ,");

		IpPacket* packet = PacketFactory::CreatePacket(infoTokens, packetTokens);

		NetworkAddress localNetworkAddress = packet->GetLocalNetworkAddress();

		Assert::AreEqual(std::string("2601:602:8d80:4f20::441f"), localNetworkAddress.IpAddress);
		Assert::AreEqual(55259, localNetworkAddress.port);
		Assert::AreEqual(std::string("www.bing.com"), ((DnsPacket*)packet)->dnsData.domainName);

		delete packet;
	}
	};
};


/*

19:16:03.247313900 PktGroupId 562949953635006, PktNumber 1, Appearance 7, Direction Rx , Type Ethernet , Component 34, Edge 1, Filter 0, OriginalSize 56, LoggedSize 56
	48-F7-C0-BA-59-91 > 3C-58-C2-BD-75-5E, ethertype ARP (0x0806), length 56: Request who-has 10.0.0.146 tell 10.0.0.1, length 42

*/