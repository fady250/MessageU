#include "packetReciever.h"

packetReciever::packetReciever() { rp = nullptr; }

packetReciever::~packetReciever() {
	if (rp != nullptr) delete rp;
}

void packetReciever::recieve(tcp::socket& sock)
{
	if (rp != nullptr) delete rp;		// start clean

	boost::system::error_code ec;
	size_t length;
	// receive the header 
	responseHeaderUnion rhu;
	length = boost::asio::read(sock, boost::asio::buffer(rhu.buf, sizeof(responseHeaderUnion)), ec);
	if (!ec || ec == boost::asio::error::eof) {}

	if (rhu.h.code == (uint16_t)responseCode::registerSucc) {
		char cid[CMN_SIZE];
		length = boost::asio::read(sock, boost::asio::buffer(cid,CMN_SIZE), ec);
		rp = new RegisterSuccessPacket(&rhu,cid);
	}
	else if (rhu.h.code == (uint16_t)responseCode::clientList) {
		uint32_t numOfClients = rhu.h.payload_size / (CMN_SIZE + MAX_NAME_SIZE);
		rp = new ClientListPacket(&rhu);
		for (int i = 0; i < numOfClients; i++) {
			char id[CMN_SIZE];
			char name[MAX_NAME_SIZE];
			length = boost::asio::read(sock, boost::asio::buffer(id, CMN_SIZE), ec);
			length = boost::asio::read(sock, boost::asio::buffer(name, MAX_NAME_SIZE), ec);
			ClientEntry ce(id, name);
			((ClientListPacket*)rp)->addEntry(ce);
		}
	}
	else if (rhu.h.code == (uint16_t)responseCode::pubKey) {
		// receive the payload 
		char cid[CMN_SIZE];
		char pub_key[PUB_KEY_LEN];
		length = boost::asio::read(sock, boost::asio::buffer(cid, CMN_SIZE), ec);
		length = boost::asio::read(sock, boost::asio::buffer(pub_key, PUB_KEY_LEN), ec);
		rp = new PubKeyResponsePacket(&rhu,cid,pub_key);
	}
	else if (rhu.h.code == (uint16_t)responseCode::msgSent) {
		// TODO what do i need to do with msg id ? 
		// receive the payload 
		char cid[CMN_SIZE];
		char mid[MSG_ID_LEN];
		length = boost::asio::read(sock, boost::asio::buffer(cid, CMN_SIZE), ec);
		length = boost::asio::read(sock, boost::asio::buffer(mid, MSG_ID_LEN), ec);
		// TODO do we need to print something here ? does it worth a new class packet ?
		//// WE HAVE THE HEADER READY
	}
	else if (rhu.h.code == (uint16_t)responseCode::msgPull) {
		uint32_t pay_size = rhu.h.payload_size;
		msgPullPayloadUnion mppu;
		// create packet
		rp = new MessagePacket(&rhu);
		while (pay_size > 0) {
			// receive the structure that is before the variable lenght message content
			length = boost::asio::read(sock, boost::asio::buffer(mppu.buf, CMN_SIZE), ec);
			MsgEntry e(&mppu);
			// receive the message 
			char* msg = new char[mppu.p.msg_size];
			length = boost::asio::read(sock, boost::asio::buffer(msg, mppu.p.msg_size), ec);
			pay_size -= (mppu.p.msg_size + sizeof(msgPullPayloadUnion));
			e.set_msg(msg);
			((MessagePacket*)rp)->addEntry(e);
		}
	}
	else if (rhu.h.code == (uint16_t)responseCode::error) {
		// nothing to receive 
		// WE HAVE THE HEADER READY
		// TODO where are we printing errror msg to the user
	}
	else {// nothing to receive
		// empty rhu , throw exception
	}
}

ResponsePacketHeader* packetReciever::getPacket() const
{
	return rp;
}


/*
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

*/