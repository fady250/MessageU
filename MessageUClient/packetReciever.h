#pragma once

#include <boost/asio.hpp>
#include "misc.h"
#include "ResponsePacketHeader.h"
#include "RegisterSuccessPacket.h"
#include "ClientListPacket.h"


using boost::asio::ip::tcp;

class packetReciever {
private:
	ResponsePacketHeader* rp;
public:
	packetReciever();
	void recieve(tcp::socket& sock);
	ResponsePacketHeader* getPacket() const;
	//uint32_t receivePayChunk(tcp::socket& soc, uint32_t bytes_left);	// returns how many bytes received 
	//payloadChunk* getPay();							// pointer for the buffers
	~packetReciever();
};