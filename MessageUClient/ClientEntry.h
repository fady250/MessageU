#pragma once

class ClientEntry {
private:
	char* client_id;
	char* client_name;
	// TODO ADD : 
	// public key 
	// symmetric key 
public:
	ClientEntry(char* id, char* name) {
		client_id = new char[CMN_SIZE];
		client_name = new char[MAX_NAME_SIZE];
		memcpy(client_id, id, CMN_SIZE);
		memcpy(client_name, name, MAX_NAME_SIZE);
	}
	ClientEntry(const ClientEntry& ce) {
		client_id = new char[CMN_SIZE];
		client_name = new char[MAX_NAME_SIZE];
		memcpy(client_id, ce.client_id, CMN_SIZE);
		memcpy(client_name, ce.client_name, MAX_NAME_SIZE);
	}
	char* getId()const {
		return client_id;
	}

	char* getName()const {
		return client_name;
	}
	~ClientEntry() {
		delete client_id;
		delete client_name;
	}
};