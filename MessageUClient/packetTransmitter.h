#pragma once

#include <boost/asio.hpp>
#include "string.h"
#include "misc.h"
#include "packetReciever.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <filesystem>

using boost::asio::ip::tcp;

class packetTransmitter {
private:
	//responsePacketHeaderConst* constHeader = nullptr;
	//PacketHeaderFlex* flexHeader = nullptr;
	//payloadChunk* payChunk = nullptr;
public:
	//packetTransmitter();
	//void buildConstHeader(enum class status st);
	//void buildFlexHeader(std::string st, uint16_t size);
	//void send(tcp::socket& soc, packetReciever* pr);
	//~packetTransmitter();
};