
#include <algorithm>
#include <vector>
#include <iostream>
#include <windows.h>
#include "PacketProcessor.h"
#include "PacketFactory.h"
#include "PacketStringTokenizer.h"

#include "IcmpPacket.h"
#include "UdpPacket.h"
#include "TcpPacket.h"

#define NUMBYTESTOREAD 4096

PacketProcessor::PacketProcessor(HANDLE fileDescriptor)
{
    // TODO: check if NULL

    PacketProcessor::fileDescriptor = fileDescriptor;
    PacketProcessor::bufferOffset = 0;
    PacketProcessor::numBytesInBuffer = 0;
    memset(PacketProcessor::pktmonBuffer, 0, BUFSIZE);
}

bool IsSpace(unsigned char c) 
{
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\f');
}

/*  Read the beginning text output from PktMon.
 *  Read 100 lines before throwing an exception.
 *  The beginning text is in the following format:

 Collected Data:
    Packet counters, packet capture

Capture Type:
    All packets

Monitored Components:
    All

Packet Filters:
    None
Processing...

*/
void PacketProcessor::ProcessPktmonStartText()
{
    int maxLinesToRead = 100;
    int linesRead = 0;
    char buffer[4096];

    while (true)
    {
        if (linesRead >= maxLinesToRead)
        {
            // throw;
            break;
        }

        PacketProcessor::LoadNextLineIntoBuffer(buffer);

        std::string line = std::string(buffer);

        std::cout << line << std::endl;

        if (line == "Processing...")
        {
            break;
        }
    }
}

IpPacket * PacketProcessor::GetNextPacket()
{
    IpPacket* packetPtr = NULL;
    while (packetPtr == NULL)
    {
        std::string infoString = GetNextInfoString();

        PacketStringTokenizer infoStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(infoString);

        // parse info string
        std::vector<std::string> infoTokens = infoStringTokenizer.GetAllTokens(" ,");

        // process tokens
        std::string packetString = GetNextPacketString();

        PacketStringTokenizer packetStringTokenizer = PacketStringTokenizer::PacketStringTokenizer(packetString);

        std::vector<std::string> packetTokens = packetStringTokenizer.GetAllTokens(" ,");

        while (!packetString.empty())
        {
            // TODO: process encapsulated packets
            packetString = GetNextPacketString();
        }

        packetPtr = PacketFactory::CreatePacket(infoTokens, packetTokens);
    }

    return packetPtr;
}

std::string PacketProcessor::GetNextInfoString()
{
    std::string infoString;

    char nextChar = NULL;

    char buf[2048];

    int length = 0;

    length = PacketProcessor::LoadNextLineIntoBuffer(buf);

    infoString = std::string(buf);

    // todo: eliminate recursion
    if (infoString == "")
    {
        length = PacketProcessor::LoadNextLineIntoBuffer(buf);
        infoString = std::string(buf);
    }

    return infoString;
}

std::string PacketProcessor::GetNextPacketString() 
{
    std::string packetString;

    char nextChar = NULL;

    char buf[2048];

    int length = 0;

    if (PeekNextChar() == '\t')
    {
        length += LoadNextLineIntoBuffer(buf);
    }

    buf[length] = '\0';

    packetString = std::string(buf);

    return packetString;
}

// loads next line defined by \n character.
// replaces newline character with null.
// returns length of cstring.
// a line with no characters other than \n will return empty string (length = 0)
int PacketProcessor::LoadNextLineIntoBuffer(char * buf)
{
    char nextChar = NULL;
    int length = 0;

    while (nextChar != '\n' && length < 2048)
    {
        nextChar = GetNextChar();
        buf[length] = nextChar;
        length++;
    }

    // replace last character (newline) with null.
    // this turns the buf variable into a cstring with a length of total bytes - 2 (because the last byte is the null terminator).
    buf[length - 2] = '\0';
    length--; // removed \n
    length--; // removed \r

    return length;
}

char PacketProcessor::GetNextChar()
{
    char nextChar = PacketProcessor::PeekNextChar();
    bufferOffset++;

    return nextChar;
}

char PacketProcessor::PeekNextChar()
{
    // check if there is data in the buffer
    if (!(PacketProcessor::bufferOffset < PacketProcessor::numBytesInBuffer))
    {
        PacketProcessor::ReadFromPktMon();

        //reset offset
        PacketProcessor::bufferOffset = 0;
    }

    char nextChar = PacketProcessor::pktmonBuffer[PacketProcessor::bufferOffset];

    return nextChar;
}

/*  Read up to 4096 bytes from PktMon into internal buffer.  
 *  Set PacketProcessor::numBytesInBuffer to numBytes returned in ReadFile. 
 *  Set PacketProcessor::numBytesInBuffer to 0 if ReadFile returns EOF. 
 */
void PacketProcessor::ReadFromPktMon()
{
    BOOL readSuccess;
    DWORD bytesRead;
    int totalBytesRead = 0;

	// each call to read will begin writing data at beginning of readBuffer
	readSuccess = ReadFile(PacketProcessor::fileDescriptor, PacketProcessor::pktmonBuffer, NUMBYTESTOREAD, &bytesRead, NULL);

	if (!readSuccess)
	{
		int error = GetLastError();
		// break;
	}

	if (bytesRead == 0)
	{
		// handle EOF
	}

	PacketProcessor::numBytesInBuffer = bytesRead;
	PacketProcessor::bufferOffset = 0;
}
