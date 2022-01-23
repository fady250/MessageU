#pragma once

#include "ResponsePacketHeader.h"
#include "ClientEntry.h"
#include <vector>

class ClientListPacket : public ResponsePacketHeader {
private:
	std::vector<ClientEntry>* lis;
public:
	ClientListPacket(responseHeaderUnion* rh) :ResponsePacketHeader(rh->h.version, rh->h.code, rh->h.payload_size) {
		lis = new std::vector<ClientEntry>;
	}
	std::vector<ClientEntry>* getPay() {
		return lis;
	}
	void addEntry(ClientEntry& ce) {
		lis->push_back(ce);
	}
	~ClientListPacket() {
		delete lis;
	}
};