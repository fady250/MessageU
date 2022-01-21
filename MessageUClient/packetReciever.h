#pragma once

#include <boost/asio.hpp>
#include <fstream>
using boost::asio::ip::tcp;

class packetReciever {

public:
	packetReciever();
	void recieve(tcp::socket& sock);
	//void receiveHeader(tcp::socket& soc);
	//uint32_t receivePaySize(tcp::socket& soc);
	//uint32_t receivePayChunk(tcp::socket& soc, uint32_t bytes_left);	// returns how many bytes received 
	//requestPacketHeaderConst* getConstHeader();		// pointer for the buffers
	//PacketHeaderFlex* getFlexHeader();		// pointer for the buffers
	//payloadChunk* getPay();							// pointer for the buffers
	//~packetReciever();
};