#pragma once

#include "requestPacketHeader.h"

class PubKeyPullPacket : public requestPacketHeader {
private:
	pubKeyPullPayload* p;
public:
	PubKeyPullPacket(char* my_id, uint16_t code, char* client_id) :requestPacketHeader(my_id, code) {
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
