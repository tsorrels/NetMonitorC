
#include "DnsPacket.h"



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

		if (dnsTokens.size() < 3)
		{
			return dnsData;
		}

		dnsData.requestId = std::stoi(dnsTokens[0]);
		dnsData.domainName = dnsTokens[2];
		// TODO: parse recordType
		// dnsData.recordType = dnsTokens[0];

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

	static bool compareDnsData(DnsData dnsData1, DnsData dnsData2)
	{
		bool isDnsData1Bigger = false;

		if (dnsData1.requestTime > dnsData2.requestTime)
			isDnsData1Bigger = true;

		return isDnsData1Bigger;
	}
