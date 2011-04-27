#ifndef KDC_H
#define KDC_H

#include <iostream>
#include "EncryptedSockets.h"
#include "Encrypter.h"

void* proxy_function(void*);

class KDC {
	
	public:
		// Instance variables
		int clientCount;
		char* sessionKey;
		char* request;
		uint64_t nonce;
		char* size;
		char** clientKeys;
		char** clientIDs;
		int kdcPort;
		int serverPort;
		string serverHostname;
		string ida;		

		Encrypter* e;
		
		// Cosntructors/destructors
		//KDC(char* newSessionKey, int newClientCount, char** newClientKeys, 
		//	int kdcPort);//, char* serverHostname, int serverPort);
		KDC(KDC_info* info);
		void execute();
		
		// Socket Stuff 
		void getFromTCPClient(TCPSocket*);
		void sendToTCPClient(TCPSocket*);
		TCPServerSocket* kdcSocket;
		TCPSocket* clientSocket;

		// Thread functions
		void* thread_function(void*);
	
};

	
#endif
