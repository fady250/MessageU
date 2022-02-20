#pragma once

#include <string>

#define VERSION 1
#define TXT		4
#define RANDOM_NAME_LEN 32

enum class userInput {Register = 110, RequestList = 120, RequestpubKey = 130, Requestmsgs = 140, SendTxtMsg = 150, RequestSymKey = 151, SendSymKey = 152, exit = 0};

enum class requestCode { exit = 0, userRegister = 1100, clientsList = 1101, pullClientPubKey = 1102, sendMsg = 1103, pullMsgs = 1104 };

enum class msgType { symKeyReq = 1, symKeySend = 2, textMsgSend = 3 };	// relevant for requestCode sendMsg

enum class responseCode { registerSucc = 2100, clientList = 2101, pubKey = 2102, msgSent = 2103, msgPull = 2104, error = 9000};

class misc {
public:
	static std::string random_string(size_t length);
	static std::string convertToString(char* a, int size); 
	static std::string convertNullTerminatedToString(char* a);
};