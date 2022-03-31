#include "Protocol.h"
#include <map>

std::map<int, Protocol::ProtoEnum> Protocol::tcpProtocolMap = {
	{443, Protocol::ProtoEnum::HTTPS},
	{53, Protocol::ProtoEnum::DNS},
	{445, Protocol::ProtoEnum::SMB},
	{135, Protocol::ProtoEnum::RPC},
};

std::map<int, Protocol::ProtoEnum> Protocol::udpProtocolMap = {
	{443, Protocol::ProtoEnum::QUIC},
	{53, Protocol::ProtoEnum::DNS}
};