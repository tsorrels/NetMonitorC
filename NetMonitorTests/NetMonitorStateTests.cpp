#include "pch.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include "../NetMonitor/NetMonitorState.h"
#include "../NetMonitor/DnsPacket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NetMonitorTests
{
	TEST_CLASS(NetMonitorStateTests)
	{
	public:
		TEST_METHOD(NetMonitorState_ProcessTcpPacket_ConnectionAdded)
		{
			TcpPacket tcpPacket = TcpPacket::TcpPacket();
			tcpPacket.packetType = PacketType::Ethernet;
			tcpPacket.transportProtocol = TransportProtocol::TCP;
			tcpPacket.ipVersion = IPVersion::IPv4;
			tcpPacket.appearance = 1;
			tcpPacket.localIpAddress = "1.1.1.1";
			tcpPacket.remoteIpAddress = "1.1.1.2";
			tcpPacket.localPort = 1001;
			tcpPacket.remotePort = 1002;
			tcpPacket.direction = Direction::Rx;

			NetMonitorState state = NetMonitorState::NetMonitorState();

			state.ProcessPacket(&tcpPacket);

			Assert::AreEqual(1, (int)state.ipConnections.allConnections.size());
			Assert::AreEqual(1001, (int)state.ipConnections.allConnections[0]->localNetworkAddress.port);
		}

		TEST_METHOD(NetMonitorState_ProcessTcpPacket_ConnectionUpdated)
		{
			TcpPacket tcpPacket1 = TcpPacket::TcpPacket();
			tcpPacket1.pktGroupId = "groupid1";
			tcpPacket1.pktNumber = 1;
			tcpPacket1.appearance = 1;
			tcpPacket1.packetType = PacketType::Ethernet;
			tcpPacket1.transportProtocol = TransportProtocol::TCP;
			tcpPacket1.ipVersion = IPVersion::IPv4;
			tcpPacket1.localIpAddress = "1.1.1.1";
			tcpPacket1.remoteIpAddress = "1.1.1.2";
			tcpPacket1.localPort = 1001;
			tcpPacket1.remotePort = 1002;
			tcpPacket1.length = 1000;
			tcpPacket1.direction = Direction::Rx;

			TcpPacket tcpPacket2 = TcpPacket::TcpPacket();
			tcpPacket2.pktGroupId = "groupid1";
			tcpPacket2.pktNumber = 2;
			tcpPacket2.appearance = 1;
			tcpPacket2.packetType = PacketType::Ethernet;
			tcpPacket2.transportProtocol = TransportProtocol::TCP;
			tcpPacket2.ipVersion = IPVersion::IPv4;
			tcpPacket2.localIpAddress = "1.1.1.1";
			tcpPacket2.remoteIpAddress = "1.1.1.2";
			tcpPacket2.localPort = 1001;
			tcpPacket2.remotePort = 1002;
			tcpPacket2.length = 1000;
			tcpPacket2.direction = Direction::Rx;

			NetMonitorState state = NetMonitorState::NetMonitorState();

			state.ProcessPacket(&tcpPacket1);
			state.ProcessPacket(&tcpPacket2);

			Assert::AreEqual(1, (int)state.ipConnections.allConnections.size());
		}

		TEST_METHOD(NetMonitorState_ProcessTcpPacket_ConnectionRxTxUpdated)
		{
			TcpPacket tcpPacket1 = TcpPacket::TcpPacket();
			tcpPacket1.pktGroupId = "groupid1";
			tcpPacket1.pktNumber = 1;
			tcpPacket1.appearance = 1;
			tcpPacket1.packetType = PacketType::Ethernet;
			tcpPacket1.transportProtocol = TransportProtocol::TCP;
			tcpPacket1.ipVersion = IPVersion::IPv4;
			tcpPacket1.localIpAddress = "1.1.1.1";
			tcpPacket1.remoteIpAddress = "1.1.1.2";
			tcpPacket1.localPort = 1001;
			tcpPacket1.remotePort = 1002;
			tcpPacket1.length = 1000;
			tcpPacket1.direction = Direction::Rx;

			TcpPacket tcpPacket2 = TcpPacket::TcpPacket();
			tcpPacket2.pktGroupId = "groupid1";
			tcpPacket2.pktNumber = 2;
			tcpPacket2.appearance = 1;
			tcpPacket2.packetType = PacketType::Ethernet;
			tcpPacket2.transportProtocol = TransportProtocol::TCP;
			tcpPacket2.ipVersion = IPVersion::IPv4;
			tcpPacket2.localIpAddress = "1.1.1.2";
			tcpPacket2.remoteIpAddress = "1.1.1.1";
			tcpPacket2.localPort = 1002;
			tcpPacket2.remotePort = 1001;
			tcpPacket2.length = 1000;
			tcpPacket2.direction = Direction::Tx;

			NetMonitorState state = NetMonitorState::NetMonitorState();

			state.ProcessPacket(&tcpPacket1);
			state.ProcessPacket(&tcpPacket2);

			Assert::AreEqual(1, (int)state.ipConnections.allConnections.size());
		}

		TEST_METHOD(NetMonitorState_ProcessDuplicatePacket_DuplicateNotAdded)
		{
			TcpPacket tcpPacket1 = TcpPacket::TcpPacket();
			tcpPacket1.pktGroupId = "groupid1";
			tcpPacket1.pktNumber = 1;
			tcpPacket1.appearance = 1;
			tcpPacket1.packetType = PacketType::Ethernet;
			tcpPacket1.transportProtocol = TransportProtocol::TCP;
			tcpPacket1.ipVersion = IPVersion::IPv4;
			tcpPacket1.localIpAddress = "1.1.1.1";
			tcpPacket1.remoteIpAddress = "1.1.1.2";
			tcpPacket1.localPort = 1001;
			tcpPacket1.remotePort = 1002;
			tcpPacket1.length = 1000;
			tcpPacket1.direction = Direction::Rx;

			TcpPacket tcpPacket2 = TcpPacket::TcpPacket();
			tcpPacket2.pktGroupId = "groupid1";
			tcpPacket2.pktNumber = 1;
			tcpPacket2.appearance = 2;
			tcpPacket2.packetType = PacketType::Ethernet;
			tcpPacket2.transportProtocol = TransportProtocol::TCP;
			tcpPacket2.ipVersion = IPVersion::IPv4;
			tcpPacket2.localIpAddress = "1.1.1.1";
			tcpPacket2.remoteIpAddress = "1.1.1.2";
			tcpPacket2.localPort = 1001;
			tcpPacket2.remotePort = 1002;
			tcpPacket2.length = 1000;
			tcpPacket2.direction = Direction::Rx;

			NetMonitorState state = NetMonitorState::NetMonitorState();

			state.ProcessPacket(&tcpPacket1);
			state.ProcessPacket(&tcpPacket2);

			Assert::AreEqual(1000, state.ipConnections.allConnections[0]->rxBytes);
			Assert::AreEqual(1, (int)state.ipConnections.allConnections.size());
		}

		/*TEST_METHOD(IpConnections_SortFourConnections_DoesSort)
		{
			NetMonitorState state = NetMonitorState::NetMonitorState();

			Connection connection1 = Connection();
			connection1.localNetworkAddress = NetworkAddress("1.1.1.1", 1);
			connection1.remoteNetworkAddress = NetworkAddress("1.1.1.2", 2);
			connection1.transportProtocol = TransportProtocol::UDP;
			connection1.rxBytes = 1000;
			connection1.txBytes = 2000;

			Connection connection2 = Connection();
			connection2.localNetworkAddress = NetworkAddress("2.1.1.1", 1);
			connection2.remoteNetworkAddress = NetworkAddress("2.1.1.2", 2);
			connection2.transportProtocol = TransportProtocol::UDP;
			connection2.rxBytes = 1;
			connection2.txBytes = 1;

			Connection connection3 = Connection();
			connection3.localNetworkAddress = NetworkAddress("3.1.1.1", 1);
			connection3.remoteNetworkAddress = NetworkAddress("3.1.1.2", 2);
			connection3.transportProtocol = TransportProtocol::UDP;
			connection3.rxBytes = 3000;
			connection3.txBytes = 5000;

			Connection connection4 = Connection();
			connection4.localNetworkAddress = NetworkAddress("4.1.1.1", 1);
			connection4.remoteNetworkAddress = NetworkAddress("4.1.1.2", 2);
			connection4.transportProtocol = TransportProtocol::UDP;
			connection4.rxBytes = 1000;
			connection4.txBytes = 2001;

			state.ipConnections.allConnections.push_back(&connection1);
			state.ipConnections.allConnections.push_back(&connection2);
			state.ipConnections.allConnections.push_back(&connection3);
			state.ipConnections.allConnections.push_back(&connection4);

			state.ipConnections.SortAllConnections();

			Connection largestConnection = state.ipConnections.allConnections[0];
			Connection smallestConnection = state.ipConnections.allConnections[3];

			Assert::AreEqual("1.1.1.1", largestConnection.localNetworkAddress.IpAddress.c_str());
			Assert::AreEqual(3000, largestConnection.rxBytes + largestConnection.txBytes);

			Assert::AreEqual("4.1.1.1", smallestConnection.localNetworkAddress.IpAddress.c_str());
			Assert::AreEqual(3001, smallestConnection.rxBytes + smallestConnection.txBytes);
		}*/

		TEST_METHOD(Connection_ThreeDnsPackets_DataSaved)
		{
			DnsPacket dnsPacket1 = DnsPacket::DnsPacket();
			dnsPacket1.pktGroupId = "groupid1";
			dnsPacket1.pktNumber = 1;
			dnsPacket1.appearance = 1;
			dnsPacket1.packetType = PacketType::Ethernet;
			dnsPacket1.transportProtocol = TransportProtocol::UDP;
			dnsPacket1.ipVersion = IPVersion::IPv4;
			dnsPacket1.localIpAddress = "1.1.1.1";
			dnsPacket1.remoteIpAddress = "1.1.1.2";
			dnsPacket1.localPort = 1001;
			dnsPacket1.remotePort = 53;
			dnsPacket1.length = 1000;
			dnsPacket1.direction = Direction::Tx;

			DnsData dnsData1 = DnsData();
			dnsData1.domainName = "www.yahoo.com";
			dnsData1.recordType = DnsRecordType::A;
			dnsData1.requestId = 100;

			dnsPacket1.dnsData = dnsData1;


			DnsPacket dnsPacket2 = DnsPacket::DnsPacket();
			dnsPacket2.pktGroupId = "groupid1";
			dnsPacket2.pktNumber = 2;
			dnsPacket2.appearance = 2;
			dnsPacket2.packetType = PacketType::Ethernet;
			dnsPacket2.transportProtocol = TransportProtocol::UDP;
			dnsPacket2.ipVersion = IPVersion::IPv4;
			dnsPacket2.localIpAddress = "1.1.1.1";
			dnsPacket2.remoteIpAddress = "1.1.1.2";
			dnsPacket2.localPort = 1001;
			dnsPacket2.remotePort = 53;
			dnsPacket2.length = 1000;
			dnsPacket2.direction = Direction::Tx;

			DnsData dnsData2 = DnsData();
			dnsData2.domainName = "www.bing.com";
			dnsData2.recordType = DnsRecordType::A;
			dnsData2.requestId = 200;

			dnsPacket2.dnsData = dnsData2;

			NetMonitorState state = NetMonitorState::NetMonitorState();
			state.ProcessPacket(&dnsPacket1);
			state.ProcessPacket(&dnsPacket2);

			Connection *dnsConnection = state.ipConnections.allConnections[0];
			
			std::string expectedDataString = "100,A,www.yahoo.com;200,A,www.bing.com;";

			Assert::AreEqual((int)Protocol::ProtoEnum::DNS, (int)dnsConnection->protocol);
			// Assert::AreEqual(expectedDataString, dnsConnection->data);
		}
	};
}
