#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "misc.h"
#include "SessionManager.h"

using namespace std;
using boost::asio::ip::tcp;

class Client {
private:
	string address;
	string port;
	SessionManager* sm = nullptr;
	void handle_user_input(string input);
public:
	Client();
	void start();
	~Client();

};