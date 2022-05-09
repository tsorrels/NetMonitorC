#include "pch.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include "../NetMonitor/NetMonitorState.h"
#include "../NetMonitor/DnsPacket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NetMonitorTests
{
	TEST_CLASS(ConnectionFactoryTests)
	{
	public:
		TEST_METHOD(ConnectionFactory_CreateConnection_KnownDstPort)
		{
			ConnectionFactory connectionFactory = ConnectionFactory();

			TcpPacket tcpPacket = TcpPacket();

			tcpPacket.localIpAddress = "1.1.1.1";
			tcpPacket.remoteIpAddress = "8.8.8.8";
			tcpPacket.localPort = 11111;
			tcpPacket.remotePort = 443;

			tcpPacket.length = 1200;
			tcpPacket.transportProtocol = TransportProtocol::TCP;
			tcpPacket.ipVersion = IPVersion::IPv4;

			Connection* connection = connectionFactory.CreateConnection(&tcpPacket);

			Assert::AreEqual((int)Protocol::ProtoEnum::HTTPS, (int)connection->protocol);	
		}

		TEST_METHOD(ConnectionFactory_CreateConnection_KnownSrcPort)
		{
			ConnectionFactory connectionFactory = ConnectionFactory();

			TcpPacket tcpPacket = TcpPacket();

			tcpPacket.localIpAddress = "8.8.8.8";
			tcpPacket.remoteIpAddress = "1.1.1.1";
			tcpPacket.localPort = 443;
			tcpPacket.remotePort = 11111;

			tcpPacket.length = 1200;
			tcpPacket.transportProtocol = TransportProtocol::TCP;
			tcpPacket.ipVersion = IPVersion::IPv4;

			Connection* connection = connectionFactory.CreateConnection(&tcpPacket);

			Assert::AreEqual((int)Protocol::ProtoEnum::HTTPS, (int)connection->protocol);
		}

		TEST_METHOD(ConnectionFactory_CreateConnection_RdpSrcPort)
		{
			ConnectionFactory connectionFactory = ConnectionFactory();

			TcpPacket tcpPacket = TcpPacket();

			tcpPacket.localIpAddress = "8.8.8.8";
			tcpPacket.remoteIpAddress = "1.1.1.1";
			tcpPacket.localPort = 443;
			tcpPacket.remotePort = 11111;

			tcpPacket.length = 1200;
			tcpPacket.transportProtocol = TransportProtocol::TCP;
			tcpPacket.ipVersion = IPVersion::IPv4;

			Connection* connection = connectionFactory.CreateConnection(&tcpPacket);

			Assert::AreEqual((int)Protocol::ProtoEnum::HTTPS, (int)connection->protocol);
		}
	};
}