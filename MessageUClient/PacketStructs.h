#pragma once
#include<stdint.h>

// defines
#define ZERO				0
#define CMN_SIZE			16		// 16 bytes		COMMON_SIZE - used in some of the many fields 
#define MAX_NAME_SIZE		255
#define PUB_KEY_LEN			160
#define SYM_KEY_LEN			CMN_SIZE
#define REGISTER_PAY_SIZE	MAX_NAME_SIZE + PUB_KEY_LEN // 415

#define MSG_ID_LEN	    4		// 
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
	char public_key[PUB_KEY_LEN];
};

struct msgSendPayload {
	char recepient_id[CMN_SIZE];
	uint8_t msg_type;
	uint32_t content_size;
	char msg_content[];			// flexible	// sizeof(MsgSendPayload) , however this memory is allocated Continuously
};

struct msgPullPayload {
	char client_id[CMN_SIZE];
	uint32_t msg_id;
	uint8_t msg_type;
	uint32_t msg_size;
};

#pragma pack(pop)

// unions :

union requestHeaderUnion
{
	requestHeader h;
	char buf[sizeof(requestHeader)];
};

union responseHeaderUnion
{
	responseHeader h;						
	char buf[sizeof(responseHeader)];
};

// payload unions :

union registerPayloadUnion
{
	registerPayload h;
	char buf[sizeof(registerPayload)];
};

union msgSendPayloadUnion
{
	msgSendPayload p;							// 6 bytes + variable lenght
	char buf[sizeof(msgSendPayload)];
};

union msgPullPayloadUnion
{
	msgPullPayload p;
	char buf[sizeof(msgPullPayload)];
};
