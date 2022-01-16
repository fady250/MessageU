#pragma once

#include <string>

#define VERSION 1
#define TXT		4
#define RANDOM_NAME_LEN 32

enum class operation { backup = 100, recover = 200, delete_file = 201, list_files = 202 };
enum class 
	status { succ_restored = 210, 
	succ_list = 211, 
	succ_backup_delete = 212, 
	err_doesnt_exist = 1001, 
	err_no_files = 1002, 
	error = 1003 };

class misc {
public:
	static std::string random_string(size_t length);
	static std::string convertToString(char* a, int size);
};