#pragma once

#include "requestPacketHeader.h"

class MsgSendPacket : public RequestPacketHeader {
private:
	msgSendPayloadUnion* p;
public:
	MsgSendPacket(const char* my_id, char* recepient_id, uint8_t type, uint32_t content_size, std::string content) :RequestPacketHeader(my_id, (uint16_t)requestCode::sendMsg) {
		// update payload size in the header
		rh->h.payload_size = sizeof(msgSendPayload) + content_size ;
		// TODO if its a file - will be sent in chunks - no need to allocate mem for file
		p = (msgSendPayloadUnion*)new char[sizeof(msgSendPayloadUnion) + sizeof(char) * content_size];
		memcpy(p->p.recepient_id, recepient_id, CMN_SIZE);
		p->p.msg_type = type;
		p->p.content_size = content_size;
		if(content_size) memcpy(p->p.msg_content, content.c_str(), content.size());
	}
	msgSendPayloadUnion* getPay() const {
		return p;
	}
	~MsgSendPacket() {
		delete p;
	}
};