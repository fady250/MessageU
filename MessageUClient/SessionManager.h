#pragma once

#include <boost/asio.hpp>
#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include "packetTransmitter.h"
#include "packetReciever.h"
#include "RegisterPacket.h"
#include "pubKeyPullPacket.h"
#include "MsgSendPacket.h"
#include "ClientEntry.h"

using boost::asio::ip::tcp;
using namespace std;

class SessionManager {
private:
	AESWrapper* aes = nullptr;
	RSAPrivateWrapper* rsapriv = nullptr;
	packetTransmitter* pt = nullptr;
	packetReciever* pr = nullptr;
	//char* my_id;	SAVED IN MY.INFO
	// vector of class that holds client name + id , filled when receive list of clients from server
	vector<ClientEntry> clients;
	const char* get_my_id() const;							// get my id from my.info
	char* get_recepient_id(string name) const;
	void handle_server_response(packetReciever* pr, RequestPacketHeader* request);
public:
	SessionManager();
	void handle_user_request(tcp::socket& sock, requestCode rc, string input);
	void handle_user_request(tcp::socket& sock, msgType mt, string input);				// for 1103 / 150 user input
	~SessionManager();
};
