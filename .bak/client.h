#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include "blowfish.h"
#include "PracticalSocket.h"
#include "ByteReorderingFunctions.h"

#define BACKLOG 100


using namespace std;

class Client {
	public:
		Client();
		Client(uint64_t, char*, bool, int);
		~Client();
		
	private:
		// Sieve and constructor stuff
		void init(uint64_t, char*, bool, int);	// used in constructor chaining
		bool done;								// bool to determine when we should exit(0)

		// Auth, Crypt and Socket stuff
		uint64_t hashF(uint64_t);					// function given to us by dr. tan
		void listenForCommunication();
		TCPSocket* getConnectionWithKDC();
		void sendInfoToKDC(TCPSocket*);
		void getInfoFromKDC(TCPSocket*);
		void getSessionKeyFromKDC();			// generates our session key
		void getAuthenticationFromPeer();
		
		TCPSocket* getConnectionWithPeer();
		void sendCyphers(TCPSocket*);
		void receiveNonce2(TCPSocket*);
		void sendMutatedNonce2(TCPSocket*);
		void receiveOkay(TCPSocket*);
		
		
		char* cipherToReceiveFromPeer1;
		char* cipherToReceiveFromPeer2;
		uint64_t nonce2ToReceiveFromPeer;
		uint64_t mutatedNonceToReceive;
		
		int localId;					// our ID (pid + host ip)
		int listenPort;					// the port we listen on
		int sendPort;					// the port we send to
		int connectedClientId;			// client's ID that we send our list to
		unsigned int recvBuffSize;
		Blowfish* bf;
		
};

#endif
