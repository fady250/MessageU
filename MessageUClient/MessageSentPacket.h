#pragma once

#include "ResponsePacketHeader.h"

class MessageSentPacket : public ResponsePacketHeader {
private:
	char* cid;
	char* mid;
public:
	MessageSentPacket(responseHeaderUnion* rh, const char* c_id, const char* m_id) :ResponsePacketHeader(rh->h.version, rh->h.code, rh->h.payload_size) {
		cid = new char[CMN_SIZE];
		mid = new char[MSG_ID_LEN];
		memcpy(cid, c_id, CMN_SIZE);
		memcpy(mid, m_id, MSG_ID_LEN);
	}
	char* getMessageId() {
		return mid;
	}
	~MessageSentPacket() {
		delete[] cid;
		delete[] mid;
	}
};