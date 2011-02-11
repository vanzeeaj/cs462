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
		
		//void greg_execute();
		
	private:
		// Sieve and constructor stuff
		void init(uint64_t, char*, bool, int);	// used in constructor chaining
		void initiate();				// called when an initiator is created
		void sieve();					// main sieve function, calls functions below
		void calcPrimes();				// calculates the primes
		void sendPrimes();				// sends our filteredList to other client
		void recvPrimes();				// blocks until ready for receive
		void printPrimes();				// prints the primes to stdout
		long fetchNextPrime(long);			// sets local prime to next prime in array
		//uint64_t* marked;
		char* marked;					// marked array
		vector<long>* foundPrimes;		// primes we've used to mark
		uint32_t userInputSize;			// the highest number we output through
		uint32_t arraySize;				// the size of our array   (this must be %8)
		long prime;						// our current prime
		bool done;						// bool to determine when we should exit(0)
		bool initiator;					// is this the client that is starting the sieve?

		// Auth, Crypt and Socket stuff
		uint64_t hashF(uint64_t);					// function given to us by dr. tan
		void listenForCommunication();
		TCPSocket* getConnectionWithKDC();
		void sendInfoToKDC(TCPSocket*);
		void getInfoFromKDC(TCPSocket*);
		void getSessionKeyFromKDC();	// generates our session key
		void getAuthenticationFromPeer();
		
		TCPSocket* getConnectionWithPeer();
		void sendCyphers(TCPSocket*);
		void receiveNonce2(TCPSocket*);
		void sendMutatedNonce2(TCPSocket*);
		void receiveOkay(TCPSocket*);
		
		void receiveCyphers(TCPSocket*);
		void sendNonce2(TCPSocket*);
		void receiveMutatedNonce2(TCPSocket*);
		void sendOkay(TCPSocket*);
		
		TCPServerSocket* peerServerSocket;
		TCPSocket* initiatorClientSocket;
		TCPSocket* activeSocket;
		
		uint64_t nonce;					// our nonce (used in authenticating)
		char* key;						// our private key (used for authenticating)
		char* sessionKey;				// our session key (used for encrypting/decrypting lists)
		char* request;
		char* cipherToSendToPeer1;
		char* cipherToSendToPeer2;
		uint64_t mutatedNonceToSend;
		
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
