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
	RSAPrivateWrapper* rsapriv = nullptr;
	packetTransmitter* pt = nullptr;
	packetReciever* pr = nullptr;
	//TODO register done boolean - wo it being true - no requests can be done except of register 
	vector<ClientEntry> clients;
	void get_my_id(char* outBuf) const;				// get my id from my.info
	string get_my_private_key() const;				// get my private key from my.info
	string get_recepient_id_by_name(string name) const;
	string get_name_by_id(char* id) const;
	void handle_server_response(packetReciever* pr, RequestPacketHeader* request);
public:
	SessionManager();
	void handle_user_request(tcp::socket& sock, requestCode rc, string input);
	void handle_user_request(tcp::socket& sock, msgType mt, string input);				// for 1103 / 150 user input
	~SessionManager();
};
