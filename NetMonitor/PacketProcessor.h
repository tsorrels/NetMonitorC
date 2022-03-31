#pragma once

#include <windows.h>
#include <string>
#include "IPPacket.h"

#define BUFSIZE 4096

class PacketProcessor
{
public:
	PacketProcessor(HANDLE fileDescriptor);

	// counters

	void Run();

	IpPacket * GetNextPacket();

	void ProcessPktmonStartText();

	IpPacket* ParsePacketString();

	std::string GetNextPacketString();

private:
	HANDLE fileDescriptor;
	CHAR pktmonBuffer[BUFSIZE];
	int bufferOffset;
	int numBytesInBuffer;
	// CHAR workingBuffer[BUFSIZE];

	void ReadFromPktMon();

	int LoadNextLineIntoBuffer(char* buf);

	std::string GetNextInfoString();

	char GetNextChar();
	char PeekNextChar();
};
