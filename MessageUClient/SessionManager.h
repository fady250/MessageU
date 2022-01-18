#pragma once

#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include "packetTransmitter.h"
#include "packetReciever.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

class SessionManager {
private:
	AESWrapper* aes;
	packetTransmitter* pt;
	packetReciever* pr;
public:
	SessionManager();
	void handle_request(tcp::socket& sock, requestCode rc, string input);
	void handle_request(tcp::socket& sock, requestCode rc, msgType mt, string input);
	~SessionManager();
};
