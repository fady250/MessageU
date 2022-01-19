#pragma once

#include <boost/asio.hpp>
#include "string.h"
#include "misc.h"
#include "PacketStructs.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <filesystem>

using boost::asio::ip::tcp;

class packetTransmitter {

public:
	packetTransmitter();
	void send(tcp::socket& sock, requestPacketHeader* header);											// for 1101/1104
	void send(tcp::socket& sock, requestPacketHeader* header, registerPacketPayload* registerPay);		// for 1100
	void send(tcp::socket& sock, requestPacketHeader* header, pubKeyPullPayload* pay);					// for 1102
	void send(tcp::socket& sock, requestPacketHeader* header, msgSendPacketPayload* msgPay);			// for 1103	
	~packetTransmitter();
};