#pragma once

#include "ResponsePacketHeader.h"
#include "ClientEntry.h"

using namespace std;

class ClientListPacket : public ResponsePacketHeader {
private:
	vector<ClientEntry>* lis;
public:
	ClientListPacket(uint8_t version, uint16_t code, uint32_t payload_size) :ResponsePacketHeader(version, code, payload_size) {
		lis = new vector<ClientEntry>;
	}
	vector<ClientEntry>* getPay() const {
		return lis;
	}
	void addEntry(ClientEntry& ce) {
		lis->push_back(ce);
	}
	~ClientListPacket() {
		delete lis;
	}
};