#pragma once

#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "RSAWrapper.h"

class SessionManager {
	// crypto 
	// pkt tx 
	// pkt rx 
public:
	SessionManager();
	void handle_request();
	~SessionManager();
};
