#pragma once

#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class SessionManager {
	// crypto 
	// pkt tx 
	// pkt rx 
public:
	SessionManager();
	void handle_request(tcp::socket& sock);
	~SessionManager();
};
