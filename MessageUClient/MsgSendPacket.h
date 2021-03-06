#pragma once

#include "requestPacketHeader.h"

class MsgSendPacket : public RequestPacketHeader {
private:
	msgSendPayloadUnion* p;
public:
	MsgSendPacket(const char* my_id, const char* recepient_id, uint8_t type, std::string content) :RequestPacketHeader(my_id, (uint16_t)requestCode::sendMsg) {
		// update payload size in the header
		rh->h.payload_size = sizeof(msgSendPayload) + content.size() ;
		p = (msgSendPayloadUnion*)new char[sizeof(msgSendPayloadUnion) + sizeof(char) * content.size()];
		memcpy(p->p.recepient_id, recepient_id, CMN_SIZE);
		p->p.msg_type = type;
		p->p.content_size = content.size();
		if(p->p.content_size) memcpy(p->p.msg_content, content.c_str(), content.size());
	}
	msgSendPayloadUnion* getPay() const {
		return p;
	}
	~MsgSendPacket() {
		delete p;
	}
};