#pragma once

#include "requestPacketHeader.h"

class PubKeyPullPacket : public RequestPacketHeader {
private:
	pubKeyPullPayload* p;
public:
	PubKeyPullPacket(const char* my_id, uint16_t code, char* client_id) :RequestPacketHeader(my_id, code) {
		p = new pubKeyPullPayload;
		memcpy(p->client_id, client_id, CMN_SIZE);
	}

	pubKeyPullPayload* getPay() const {
		return p;
	}
	~PubKeyPullPacket() {
		delete p;
	}
};
