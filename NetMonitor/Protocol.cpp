#include "Protocol.h"
#include <map>

std::map<int, Protocol::ProtoEnum> Protocol::tcpProtocolMap = {
	{443, Protocol::ProtoEnum::HTTPS},
	{53, Protocol::ProtoEnum::DNS},
	{445, Protocol::ProtoEnum::SMB},
	{135, Protocol::ProtoEnum::RPC},
	{1433, Protocol::ProtoEnum::MSSQL},
	{3389, Protocol::ProtoEnum::RDP},
	{5985, Protocol::ProtoEnum::RDP},
	{5986, Protocol::ProtoEnum::RDP},
};

std::map<int, Protocol::ProtoEnum> Protocol::udpProtocolMap = {
	{443, Protocol::ProtoEnum::QUIC},
	{53, Protocol::ProtoEnum::DNS},
	{3389, Protocol::ProtoEnum::RDP},
	{1900, Protocol::ProtoEnum::SSDP}
};