#include "packetReciever.h"
#include "misc.h"

using namespace std;

/*
packetReciever::packetReciever() {}

void packetReciever::receiveHeader(tcp::socket& soc){
 
	boost::system::error_code ec;
	constHeader = new requestPacketHeaderConst;

	// receive the known len header data up to op field
	size_t length = boost::asio::read(soc, boost::asio::buffer(constHeader->buf, REQ_CONST_H_LEN),ec);
	if (!ec || ec == boost::asio::error::eof) {}
	
	if (constHeader->hc.op == (int)operation::list_files) return;		// this operation doesnt have flex header 

	// get the name_len 
	char namelen[2];
	boost::asio::read(soc, boost::asio::buffer(namelen, 2), ec);
	unsigned short len = *(unsigned short*)namelen;
	// allocate with flexible buffer 
	flexHeader = (PacketHeaderFlex*)new char[sizeof(PacketHeaderFlex) + sizeof(char)*len];
	flexHeader->hf.namelen = len;
	// receive the file name 
	boost::asio::read(soc, boost::asio::buffer(flexHeader->hf.file_name, len),ec);
}

uint32_t packetReciever::receivePaySize(tcp::socket& soc) {
	
	boost::system::error_code ec;
	// receive the payload size
	char* size = new char[PAY_LEN];
	// receive the payload lenght
	size_t length = boost::asio::read(soc, boost::asio::buffer(size, PAY_LEN), ec);
	unsigned int pay_size = *(int*)size;
	return *(uint32_t*)size;
}

uint32_t packetReciever::receivePayChunk(tcp::socket& soc, uint32_t bytes_left) {
	// if not allocated yet - will be cleared once destructing the packetReceiver object
	if (payChunk == nullptr) payChunk = new payloadChunk;
	boost::system::error_code ec;
	// receive BY chunks - as long as the bytes left are bigger than 1 chuck --> receive chunk , else receive the remaining 
	uint32_t pay_size = bytes_left > CHUNK_SIZE ? CHUNK_SIZE : bytes_left;
	size_t length = boost::asio::read(soc, boost::asio::buffer(payChunk->data, pay_size), ec);
	return length;
}

// getters below :

requestPacketHeaderConst* packetReciever::getConstHeader() {
	return constHeader;
}

PacketHeaderFlex* packetReciever::getFlexHeader() {
	return flexHeader;
}

payloadChunk* packetReciever::getPay() {
	return payChunk;
}

packetReciever::~packetReciever() {
	if (constHeader != nullptr) delete constHeader;
	if (flexHeader != nullptr) delete flexHeader;
	if (payChunk != nullptr) delete payChunk;
}
*/