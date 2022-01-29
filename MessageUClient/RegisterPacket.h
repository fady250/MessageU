#pragma once

#include "RequestPacketHeader.h"

class RegisterPacket : public RequestPacketHeader {
private:
	registerPayloadUnion* p;
public:
	RegisterPacket(uint16_t code, std::string name,const char* pubkey) :RequestPacketHeader(code) {
		// update payload size 
		rh->h.payload_size = REGISTER_PAY_SIZE;
		p = new registerPayloadUnion;
		size_t s = name.size() < MAX_NAME_SIZE ? name.size() : MAX_NAME_SIZE;
		memcpy(p->h.name, name.c_str(), s);
		p->buf[s] = '\0';
		memcpy(p->h.public_key, pubkey, PUB_KEY_LEN);
		// TODO clean 
		std::cout << "public key that was generated and sent : "  << std::endl << pubkey << std::endl;
	}
	registerPayloadUnion* getPay() const {
		return p;
	}
	~RegisterPacket() {
		delete p;
	}
};