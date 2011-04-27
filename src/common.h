#ifndef NETWORKS_COMMON_H
#define NETWORKS_COMMON_H

#include <iostream>

const uint32_t recvBuffSize = 1000;
const uint32_t maxKeyLen = 16;

struct Packet {
	uint64_t id;
	char* payload;
};

typedef uint64_t Ack;

//INFORMATION FOR THE KDC
struct KDC_info{
	char* sessionKey;
	int clientCount;
	char** clientServerKeys;
	int kdcPort;
};



//INFORMATION FOR THE CLIENT
struct client_info{
	char* kdcIP;  //had to change here.
	int kdcPort;
	char* clientIP;  //had to change here.
	int clientPort; 
	char* serverIP;  //had to change here.
	int serverPort;
	uint64_t nonce;
	char* keyA;
	char* algorithm;  //had to change here.
	double packetLossRate;
	double corruptedPacketRate;
	uint64_t* droppedPackets;
	int droppedPacketCount;
	uint64_t* corruptedPackets;		
	int corruptedPacketCount;
};


//INFORMATION FOR THE SERVER
struct server_info{
	int serverPort;
	char* serverIP;  //had to change here.
	char* keyB;
	uint64_t nonce;
};

//INFORMATION FOR BOTH CLIENT AND SERVER - SPECIFIC PER FILE TRANSFER
//INFORMATION IN HERE WILL NEED TO BE SENT TO SERVER FROM CLIENT AS FIRST (FEW) PACKET(S)
struct shared_info{
	uint64_t windowSize;
	double ackLossRate;
	char* fileToSend;  //had to change here.
	int packetSize;
	uint64_t* droppedAcks;
	int droppedAckCount;		
	uint64_t CRCcode;
};



#endif
