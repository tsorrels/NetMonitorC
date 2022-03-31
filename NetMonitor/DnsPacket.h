#pragma once

#include <vector>
#include <string>
#include <time.h>
#include <chrono>
#include <stdlib.h>

#include "PacketStringTokenizer.h"
#include "UdpPacket.h"

enum class DnsRecordType
{
	A,
	AAAA,
	Other
};

class DnsData
{
public:
	int requestId;
	std::string domainName;
	DnsRecordType recordType;
	std::chrono::time_point<std::chrono::system_clock> requestTime;
	std::string resolverAddress;

	static DnsRecordType FromRecordTypeString(std::string recordTypeString)
	{
		if (recordTypeString == "A")
			return DnsRecordType::A;

		if (recordTypeString == "AAAA")
			return DnsRecordType::AAAA;

		return DnsRecordType::Other;
	}

	static std::string ToRecordTypeString(DnsRecordType type)
	{
		std::string recordTypeString;
		switch (type)
		{
		case DnsRecordType::A:
			recordTypeString = "A";
			break;

		case DnsRecordType::AAAA:
			recordTypeString = "AAAA";
			break;

		case DnsRecordType::Other:
		default:
			recordTypeString = "Other";
		}

		return recordTypeString;
	}

	static DnsData FromSingleDataString(std::string singleDnsDataString)
	{
		DnsData dnsData;

		PacketStringTokenizer tokenizer = PacketStringTokenizer(singleDnsDataString);
		std::vector<std::string> dnsTokens = tokenizer.GetAllTokens(",");

		if (dnsTokens.size() < 5)
		{
			return dnsData;
		}

		dnsData.requestId = std::stoi(dnsTokens[0]);
		dnsData.domainName = dnsTokens[2];
		dnsData.resolverAddress = dnsTokens[3];
		dnsData.recordType = FromRecordTypeString(dnsTokens[1]);

		std::string timeTString = dnsTokens[4];
		time_t timeT = std::stoll(timeTString);
		dnsData.requestTime = std::chrono::system_clock::from_time_t(timeT);

		return dnsData;
	}

	static std::vector<DnsData> FromMultipleDataString(std::string dnsDataString)
	{
		std::vector<DnsData> dnsDatas;

		PacketStringTokenizer tokenizer = PacketStringTokenizer(dnsDataString);

		std::vector<std::string> dnsDataStrings = tokenizer.GetAllTokens(";");

		for (int i = 0; i < dnsDataStrings.size(); i++)
		{
			std::string dnsDataString = dnsDataStrings[i];
			DnsData data = FromSingleDataString(dnsDataString);
			dnsDatas.push_back(data);
		}

		return dnsDatas;
	}

	static bool CompareDnsData(DnsData dnsData1, DnsData dnsData2)
	{
		return dnsData1.requestTime > dnsData2.requestTime;
	}
};

class DnsPacket : public UdpPacket
{
public:

	// for unit testing
	DnsPacket() : UdpPacket()
	{

	}

	DnsPacket(std::vector<std::string> infoTokens, std::vector<std::string> ipTokens, std::vector<std::string> protoTokens) : UdpPacket(infoTokens, ipTokens, protoTokens)
	{
		// TODO: handle DNS reponse
		if (direction == Direction::Rx)
		{
			packetType = PacketType::Other;
			return;
		}

		if (protoTokens.size() < 3)
		{
			packetType = PacketType::Other;
			return;
		}

		std::string requestIdString = protoTokens[0];
		std::string recordTypeString = protoTokens[1];
		std::string domainNameString = protoTokens[2];

		// remove training '.' from www.bing.com.
		this->dnsData.domainName = domainNameString.substr(0, domainNameString.length() - 1);
		this->dnsData.requestId = std::stoi(requestIdString);
		this->dnsData.requestTime = std::chrono::system_clock::now();
		this->dnsData.resolverAddress = remoteIpAddress;

		if (recordTypeString == "A?")
		{
			this->dnsData.recordType = DnsRecordType::A;
		}
		else if (recordTypeString == "AAAA?")
		{
			this->dnsData.recordType = DnsRecordType::AAAA;
		}
		else
		{
			this->dnsData.recordType = DnsRecordType::Other;
		}
	};

	virtual std::string GetData()
	{
		std::string recordTypeString;

		switch (this->dnsData.recordType)
		{
		case DnsRecordType::A:
			recordTypeString = "A";
			break;

		case DnsRecordType::AAAA:
			recordTypeString = "AAAA";
			break;

		case DnsRecordType::Other:
		default:
			recordTypeString = "";
		}

		time_t timeT = std::chrono::system_clock::to_time_t(this->dnsData.requestTime);

		auto timeTString = std::to_string(timeT);

		std::string dnsData = std::to_string(this->dnsData.requestId) + "," + recordTypeString + "," + this->dnsData.domainName + "," + GetRemoteNetworkAddress().IpAddress + "," + timeTString +";";

		return dnsData;
	}

	DnsData dnsData;
};
