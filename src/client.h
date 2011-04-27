#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <fstream>
#include "EncryptedSockets.h"
#include "common.h"
#include "Encrypter.h"


class Client {
	public:
		// constructors, deconstructors
		Client();
		//Client(string kdcHostname, int kdcPort, string clientHostname, 
		//	int clientPort, string serverHostname, int serverPort, 
		//	uint64_t nonce, char* keyA, string fileToSend, int packetSize);	
		Client(client_info* info, shared_info* sharedInfo);
		~Client();
		void initiate();
		
		// Instance variables
		uint64_t nonce;					// our nonce (used in authenticating)
		char* key;						// our private key (used for authenticating)
		char* sessionKey;				// our session key (used for encrypting/decrypting lists)
		char* cipherToSendToServer1;
		uint64_t cipher1Length;
		uint64_t cipher1LengthUnenc;
		char* cipherToSendToServer2;
		uint64_t cipher2Length;
		uint64_t cipher2LengthUnenc;
		uint64_t nonce2ReceivedFromServer;
		uint64_t encryptedAndMutatedNonceToSend;
		string kdcHostname;				// input from command line
		string serverHostname;			// input from command line
		int kdcPort;					// input from command line
		int clientPort;					// input from command line
		int serverPort;					// input from command line
		string clientHostname;			// generated by shell command
		unsigned int recvBuffSize;
		TCPSocket* activeSocket;
		string idb;						//this will be server ip:port
		Encrypter* e;
		
		// Auth, Crypt and Socket functions
		uint64_t hashF(uint64_t);				// function given to us by dr. tan
		TCPSocket* getConnectionWithKDC();
		void getAuthenticationInfoFromKDC();			// generates our session key
		void authenticateWithServer();
		
		TCPSocket* getConnectionWithServer();
		void sendCyphers(TCPSocket*);
		void receiveNonce2(TCPSocket*);
		void sendMutatedNonce2(TCPSocket*);
		void receiveOkay(TCPSocket*);
		
		// FTP Variables and functions.
		EncryptedUDPSocket* udpSock;
		string fileToSend;
		ifstream theIfstream;
		int packetSize;
		long currPacketId;
		string algorithm;
		long windowSize;
		
		void startFTP();
		void initUDPSocket();
		void beginSend();
		void runGoBackN();
		void runSR();
		void readNextPacketFromFile(Packet* p);
	
	private:
		// Helper functions
		void sendInfoToKDC(TCPSocket*);	
		void getInfoFromKDC(TCPSocket*);
};

#endif
