#pragma once
#include<stdint.h>

// defines
#define ZERO				0
#define CMN_SIZE			16		// 16 bytes		COMMON_SIZE - used in some of the many fields 
#define MAX_NAME_SIZE		255
#define PUB_KEY_SIZE		160
#define REGISTER_PAY_SIZE	415

//#define PAY_LEN	        4		// payload size field
#define CHUNK_SIZE		1024	// payload chunk size in bytes 

#pragma pack(push,1)

struct requestHeader {	
	char client_id[CMN_SIZE];
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

struct responseHeader {	
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

struct payloadChunk {							// used for message content in 1103 in case of file - so we dont allocate memory as big as the file size 
	char data[CHUNK_SIZE];
};

// payload structs here, according to the request 

struct registerPayload {
	char name[MAX_NAME_SIZE];
	char public_key[PUB_KEY_SIZE];
};

struct pubKeyPullPayload {				// no union for this struct
	char client_id[CMN_SIZE];
};

struct msgSendPayload {
	uint8_t client_id[CMN_SIZE];
	uint8_t msg_type;
	uint32_t content_size;
	char msg_content[];			// flexible	// sizeof(MsgSendPayload) = 6 , however this memory is allocated Continuously
};

#pragma pack(pop)

// unions :

union requestPacketHeader
{
	requestHeader h;
	char buf[sizeof(requestHeader)];
};

union responsePacketHeader
{
	responseHeader h;						
	char buf[sizeof(responseHeader)];
};

// payload unions :

union registerPacketPayload
{
	registerPayload h;
	char buf[sizeof(registerPayload)];
};

union msgSendPacketPayload
{
	msgSendPayload p;							// 6 bytes + variable lenght
	char buf[sizeof(msgSendPayload)];
};

