#include "kdc.h"
#include "common.h"
#include "PracticalSocket.h"
#include "blowfish.h"
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <memory.h>
#include <cstring>

const uint32_t recvBuffSize = RCVBUFSIZE;

KDC::KDC(char* newSessionKey, int newClientCount, char** newClientKeys) {
	sessionKey = newSessionKey;
	clientCount = newClientCount;
	clientKeys = newClientKeys;
	
	size    = new char[recvBuffSize];
	request = new char[recvBuffSize];
	//nonce   = new char[recvBuffSize];

	ida     = "23547137288161";
	idb     = "23458137288161";
	
	kdcSocket = NULL;
}

void* proxy_function(void* kdc_ptr) {
	KDC* k = static_cast<KDC*>(kdc_ptr);
	k->thread_function(k->clientSocket);
	return NULL;
}

void KDC::execute() {
	cout << "KDC initiated" << endl;
	unsigned short myPort = KDC_PORT;
	try {
		kdcSocket = new TCPServerSocket(myPort);
		
		while (true) {
			// Accept incoming connections (blocking)
			clientSocket = kdcSocket->accept();
	
			// Connection initiated, time to do things.
			cout << "Client connected..." << endl;
			// Fork a new thread to handle the connection
			pthread_t threadID;
			if (pthread_create(&threadID, NULL, proxy_function, this) != 0) {
				cerr << "Unable to create thread" << endl;
				exit(1);
			}
		}
		// NEVER REACH THIS
		// would normally delete kdcSocket, but since we don't ever exit cleanly we don't.
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	
}

void KDC::getFromTCPClient(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	uint16_t f = 0;
	sock->recv(&f, 4);
//	ntoh(&f);
//	bf->Decrypt(&f);
	f = ntohs(f);
//	reorderBytesToBigEndian(&f);
	//cout << "received len:" << f << endl;
	char* recvBuf1 = new char [f];
	sock->recv(recvBuf1, f);
	memcpy(request,recvBuf1,f);
	//cout << "grabbed request:" << request << endl;
	cout << "Received from A: (Step 1)" << endl;
	cout << "  Req = '" << request << "' (len = " << f << ")" << endl;
	
	// Same as above, except for second transmission
	uint32_t s;
	sock->recv(&s, 4);
	s = ntohs(s);
	//cout << "received len:" << s << endl;
	sock->recv(&nonce, 8);
	nonce = ntohll(nonce);
	//cout << "grabbed nonce:" << nonce << endl;
	cout << "  N1 = '" << nonce << "' (len = " << s << ")" << endl;

	delete (recvBuf1);
	//cout << "finished recv" << endl;

}

void KDC::sendToTCPClient(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint32_t sessionKeySize = strlen(sessionKey);
	uint32_t requestSize = strlen(request);
	uint32_t nonceSize = 8;
	uint32_t cipher1Size = sessionKeySize+1;
	uint32_t idaSize = strlen(ida);

	char* cipherToPeer1 = new char [cipher1Size];
	cipherToPeer1 = sessionKey + '\0';
	char* cipherToPeer2 = new char [idaSize];
	cipherToPeer2 = ida;

	//cout << "Everything initialized, starting sends..." << endl;

	try {
		// 5 buffers being sent in total.  Reordering the sizes so
		// the goonies can understand, then sending them, then
		// sending the buffers.
		// Setting up variables we'll need for blowfish and endian stuff
		uint32_t sendSize;
		//uint32_t cryptedSendSize;
		//char* cryptedSendBuff;
	//	bf->Set_Passwd(&clientKeys[0]);		// set the blowfish to keya
	/*	
		// SEND 1
		if (sessionKeySize%8) {
			cryptedSendSize = sessionKeySize/8 + sessionKeySize%8;
		} else {
			cryptedSendSize = sessionKeySize;
		}
		cryptedSendBuff = new char [cryptedSendSize];
		memcpy(cryptedSendBuff, sessionKey, sessionKeySize);
	//	bf->Encrypt((void*)&cryptedSendBuff, cryptedSendSize);
		sendSize = reorderBytesToLittleEndian(&sessionKeySize);
		sock->send(&sendSize,4);
		sendSize = reorderBytesToLittleEndian(&cryptedSendSize);
		sock->send(&sendSize,4);
		sock->send(cryptedSendBuff, reorderBytesToBigEndian(&cryptedSendSize));
		delete (cryptedSendBuff);
*/

		// SEND 1
		sendSize = htonl(sessionKeySize);
		sock->send(&sendSize, 4);
		sock->send(sessionKey, htonl(sessionKeySize));
		
		// SEND 2
		sendSize = reorderBytesToLittleEndian(&requestSize);
		// TODO ENCRYPT WITH Ka
		sock->send(&sendSize, 4);
		sock->send(request, reorderBytesToBigEndian(&requestSize));

		// SEND 3
		sendSize = reorderBytesToLittleEndian(&nonceSize);
		uint64_t sendNonce = nonce;
		reorderLong(&sendNonce);
		// TODO ENCRYPT WITH Ka
		sock->send(&sendSize, 4);
		sock->send(&sendNonce, reorderBytesToBigEndian(&nonceSize));
		

		// SEND 4
		sendSize = reorderBytesToLittleEndian(&cipher1Size);
		// TODO ENCRYPT WITH Kb, then Ka
		sock->send(&sendSize, 4);
		sock->send(cipherToPeer1, reorderBytesToBigEndian(&cipher1Size));

		// SEND 5
		sendSize = reorderBytesToLittleEndian(&idaSize);
		// TODO ENCRYPT WITH Kb, then Ka
		sock->send(&sendSize, 4);
		sock->send(cipherToPeer2, reorderBytesToBigEndian(&idaSize));
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 2)" << endl;
	cout << "  Eka[Ks||Request||N1||Ekb(Ks, IDa)]" << endl;
}

//void KDC::create_thread() {
	
//}

void* KDC::thread_function(void* clntSock) {
	// Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self());
	TCPSocket* sock = (TCPSocket*) clntSock;

	// Error catching and screen output
	cout << "Handling client ";
	try {
		cout << sock->getForeignAddress() << ":";
	} catch (SocketException e) {
		cerr << "Unable to get foreign address" << endl;
	}
	try {
		cout << sock->getForeignPort();
	} catch (SocketException e) {
		cerr << "Unable to get foreign port" << endl;
	}
	cout << " with thread " << pthread_self() << endl;

	// Perform the authentication
	getFromTCPClient(sock);
	sendToTCPClient(sock);

	cout << "Authentication with client complete. Terminating thread " << pthread_self() << endl;
	// Close socket
	delete sock;
	clntSock = NULL;
	return NULL;
}

uint64_t KDC::reorderLong(uint64_t* i) {
	unsigned char* chars = (unsigned char*)i;
	uint64_t j = 0;
	
	// Performs the swap
	swap(chars[0],chars[7]);
	swap(chars[1],chars[6]);
	swap(chars[2],chars[5]);
	swap(chars[3],chars[4]);
	
	// Casts back to a uint32_t
	j = *((uint64_t*) chars);
	i = &j;
	return j;
	
}

uint32_t KDC::reorderBytes(uint32_t* i) {
	// WORKS BOTH WAYS
	// moving bytes like so
	// [0] [1] [2] [3]
	// to
	// [3] [2] [1] [0]
	//unsigned int temp = *i;
	unsigned char* chars = (unsigned char*)i;
	uint32_t j = 0;
	
	// Performs the swap
	swap(chars[0],chars[3]);
	swap(chars[1],chars[2]);
	
	// Casts back to a uint32_t
	j = *((uint32_t*) chars);
	i = &j;
	
/*
	// moving bytes like so
	// [a] [b] [c] [d]
	// to
	// [d] [c] [b] [a]
	unsigned char a = *i & 0xFF000000;
	unsigned char b = *i & 0xFF0000;
	unsigned char c = *i & 0xFF00;
	unsigned char d = *i & 0xFF;
	cout << "sizeof(char):" << sizeof(char) << endl;
	cout << "a:" << (int)a << endl;
	cout << "b:" << (int)b << endl;
	cout << "c:" << (int)c << endl;
	cout << "d:" << (int)d << endl;
	
	uint32_t j;
	j += a;
	j += (b<<8);
	j += (c<<16);
	j += (d<<24);
	cout << "i:" << *i << " reordered to " << j << endl;
	*i = j;
*/
	chars = NULL;
	return j;
}

uint32_t KDC::reorderBytesToLittleEndian(uint32_t* i) {
	return reorderBytes(i);
}

uint32_t KDC::reorderBytesToBigEndian(uint32_t* i){
	return reorderBytes(i);
}

