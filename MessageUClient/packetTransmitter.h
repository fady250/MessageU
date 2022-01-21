#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <boost/asio.hpp>
#include "string.h"
#include "misc.h"
#include "PacketStructs.h"
#include "requestPacketHeader.h"
#include "RegisterPacket.h"
#include "pubKeyPullPacket.h"
#include "MsgSendPacket.h"

using boost::asio::ip::tcp;

class packetTransmitter {

public:
	packetTransmitter();
	void send(tcp::socket& sock, requestPacketHeader* rp);
	//void send(tcp::socket& sock, requestPacketHeader* header);											// for 1101/1104
	//void send(tcp::socket& sock, requestPacketHeader* header, registerPayloadUnion* registerPay);		// for 1100
	//void send(tcp::socket& sock, requestPacketHeader* header, pubKeyPullPayload* pay);					// for 1102
	//void send(tcp::socket& sock, requestPacketHeader* header, msgSendPacketPayload* msgPay);			// for 1103	
	~packetTransmitter();
};