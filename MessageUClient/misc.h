#pragma once

#include <string>

#define VERSION 1
#define TXT		4
#define RANDOM_NAME_LEN 32

enum class requestCode { userRegister = 1100, clientsList = 1102, sendMsg = 1103, pullMsgs = 1104 };
enum class msgType { symKeyReq = 1, symKeySend = 2, textMsgSend = 3, fileSend = 4 };


class misc {
public:
	static std::string random_string(size_t length);
	static std::string convertToString(char* a, int size);
};