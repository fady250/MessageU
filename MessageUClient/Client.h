#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include "misc.h"
#include "SessionManager.h"

using namespace std;

class Client {
private:
	string address;
	string port;
	SessionManager* sm = nullptr;
	void validate_user_input(string input);
public:
	Client();
	void start();
	~Client();

};