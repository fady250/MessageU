#pragma once

class ClientEntry {
private:
	char* client_id = nullptr;
	char* client_name = nullptr;
	unsigned char* sym_key = nullptr;
	unsigned char* pub_key = nullptr;
public:
	ClientEntry() {
		client_id = new char[CMN_SIZE];
		client_name = new char[MAX_NAME_SIZE];
		sym_key = new unsigned char[SYM_KEY_LEN];
		pub_key = new unsigned char[PUB_KEY_LEN];
	}
	ClientEntry(char* id, char* name) : ClientEntry() {
		memcpy(client_id, id, CMN_SIZE);
		memcpy(client_name, name, MAX_NAME_SIZE);
	}
	ClientEntry(const ClientEntry& ce) : ClientEntry() {
		memcpy(client_id, ce.client_id, CMN_SIZE);
		memcpy(client_name, ce.client_name, MAX_NAME_SIZE);
	}
	char* getId()const {		// TODO change to returning string 
		return client_id;
	}
	char* getName()const {
		return client_name;
	}

	unsigned char* get_sym_key() {
		return sym_key;
	}

	unsigned char* get_pub_key() {
		return sym_key;
	}

	void set_sym_key(char* sk) {
		memcpy(sym_key, sk, SYM_KEY_LEN);
	}
	void set_pub_key(char* pk) {
		memcpy(pub_key, pk, PUB_KEY_LEN);
	}

	~ClientEntry() {
		delete client_id;
		delete client_name;
		delete sym_key;
		delete pub_key;
	}
};