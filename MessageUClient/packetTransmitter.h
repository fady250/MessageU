#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <boost/asio.hpp>
#include "string.h"
#include "misc.h"
#include "RequestPacketHeader.h"
#include "RegisterPacket.h"
#include "pubKeyPullPacket.h"
#include "MsgSendPacket.h"

using boost::asio::ip::tcp;

class packetTransmitter {

public:
	packetTransmitter() {};
	void send(tcp::socket& sock, RequestPacketHeader* rp);
	~packetTransmitter() {};
};