#pragma once

#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include "packetTransmitter.h"
#include "packetReciever.h"
#include <boost/asio.hpp>
#include "RegisterPacket.h"
#include "pubKeyPullPacket.h"
#include "MsgSendPacket.h"

using boost::asio::ip::tcp;
using namespace std;

class SessionManager {
private:
	AESWrapper* aes = nullptr;
	RSAPrivateWrapper* rsapriv = nullptr;
	packetTransmitter* pt = nullptr;
	packetReciever* pr = nullptr;
	char* my_id;
	//TODO vector of class that holds client name + id , filled when receive list of clients from server
	char* get_recepient_id(string name) const;

public:
	SessionManager();
	void handle_request(tcp::socket& sock, requestCode rc, string input);
	void handle_request(tcp::socket& sock, msgType mt, string input);				// for 1103 / 150 user input
	~SessionManager();
};
