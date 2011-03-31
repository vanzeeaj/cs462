#include "client.h"

using namespace std;

Client::Client(){
	this->udpSock = new EncryptedUDPSocket();
}

Client::Client(char* kdcHostname, int kdcPort, char* clientHostname, 
			int clientPort, char* serverHostname, int serverPort, 
			uint64_t nonce, char* keyA) {
	this->kdcHostname = kdcHostname;
	this->kdcPort = kdcPort;
	this->clientPort = clientPort;
	this->nonce = nonce;
	this->key = keyA;
	this->sessionKey = new char[maxKeyLen]();
		cout << "sup" << endl;

	this->udpSock = new EncryptedUDPSocket();
		cout << "yeah" << endl;

	this->clientHostname = clientHostname;
	this->serverHostname = serverHostname;
	this->serverPort = serverPort;
	cout << "sup" << endl;
	this->idb = udpSock->hostMap[clientHostname];
	cout << "yeah" << endl;
	idb.append(":");
	char* temp = new char[sizeof(int) * 8 + 1];
	snprintf(temp, sizeof(temp), "%d", clientPort);
	idb.append(temp);
	delete(temp);
}

void Client::initiate(){
	// Start convo with the KDC
	getAuthenticationInfoFromKDC();
	// Auth with Server
	authenticateWithServer();

}

uint64_t Client::hashF(uint64_t nonce) {
	const long A = 48271; 
	const long M = 2147483647; 
	const long Q = M/A; 
	const long R = M%A; 

	static long state = 1; 
	long t = A * (state % Q) - R * (state / Q); 

	//cout << t << "=t" << endl;

	if (t > 0) 
		state = t; 
	else 
		state = t + M; 

	uint64_t retl = (uint64_t)(((double) state/M) * nonce);
	return retl;

}

	/*
	 *	
	 *	BEGIN AUTHENTICATION WITH KDC
	 *	
	 */

void Client::getAuthenticationInfoFromKDC() {
	TCPSocket* kdcSocket = getConnectionWithKDC();
	sendInfoToKDC(kdcSocket);
	getInfoFromKDC(kdcSocket);
	delete (kdcSocket);
}

TCPSocket* Client::getConnectionWithKDC() {
	try {
		// Establish connection with the echo server
		string servAddress = udpSock->hostMap[kdcHostname];
		TCPSocket* clientSocket = new TCPSocket(servAddress, kdcPort);
		return clientSocket;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	return NULL;
}

	
void Client::sendInfoToKDC(TCPSocket* sock) {
	
	//cout << "Sending request to KDC" << endl;

	// Our 2 buffers we want to send and their lengths

	unsigned int firstTransLen = idb.size();
	unsigned int secondTransLen = 8; //8 because it's a uint_64

	try {
		// Send the length to the socket, and then the buffer, for both items
		cout << "sending " << firstTransLen << " bytes:" << idb << endl;
		
		sock->send(&firstTransLen, 4);	
		sock->send(idb.c_str(), firstTransLen);
		cout << "sending " << secondTransLen << " bytes:" << nonce << endl;
		
		sock->send(&secondTransLen, 4);
		
		sock->send(&nonce, secondTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to KDC: (Step 1)" << endl;
	cout << "  Req = '" << idb << "'" << endl;
	cout << "  N1 = '" << nonce << "'" << endl;
	//cout << "sent stuff" << endl;
	//delete (firstTrans);
	
}

void Client::getInfoFromKDC(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
	int i=0;
	int transCount = 5;
	char** recvBuff = new char*[transCount];
	uint64_t recvNonce;
	
	try {
		// Loops until we receive 5 different buffers
		while (i<transCount){
			unsigned int s;					// size of the incoming buffer
			if (i==2){
				recvBuff[i] = new char [4];
				cout << "waiting for nonce size\n";
				flush(cout);
				sock->recv(&s, 4);
				sock->recv(&recvNonce, s);
			} else {
				sock->recv(&s, 4);
				recvBuff[i] = new char [s];		// create the buffer with that size
				sock->recv(recvBuff[i],s);		// receive the buffer
				cout << "received:" << recvBuff[i] << ", size:" << s << ",iteration " << i << endl;
			}
			i++;
		}
		cout << "Received from KDC: (Step 2)" << endl;
		cout << "  recvBuff[0] = '" << recvBuff[0] << "'" << endl;
		cout << "  recvBuff[1] = '" << recvBuff[1] << "'" << endl;
		cout << "  recvBuff[2] = '" << recvNonce << "'" << endl;
		cout << "  recvBuff[3] = '" << recvBuff[3] << "'" << endl;
		cout << "  recvBuff[4] = '" << recvBuff[4] << "'" << endl;
		cout << "  Eka[Ks] = '" << recvBuff[0] << "'" << endl;
		// TODO DECRYPT RECVBUFF[0-4] WITH Ka
		sessionKey = recvBuff[0];
		if (strcmp(recvBuff[1], request)) {
			cerr << "Did not receive our request back.  Terminating" << endl;
			exit(1);
		}
		if (recvNonce != nonce) {
			cerr << "Did not receive our nonce back.  Terminating" << endl;
			exit(1);	
		}
		cipherToSendToServer1 = recvBuff[3];
		cipherToSendToServer2 = recvBuff[4];
		
		// change this after decryption done:
		cout << "  Ks(decrypted) = '" << recvBuff[0] << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	delete [] recvBuff;
}

	/*
	 *	
	 *	END AUTHENTICATION WITH KDC
	 *	
	 */
	 
	/*
	 *	
	 *	BEGIN AUTHENTICATION WITH SERVER
	 *	
	 */

void Client::authenticateWithServer() {
	TCPSocket* ServerSocket = getConnectionWithServer();
	sendCyphers(ServerSocket);
	receiveNonce2(ServerSocket);
	sendMutatedNonce2(ServerSocket);
	receiveOkay(ServerSocket);
	cout << "Finished receiving authentication from Server" << endl;
	activeSocket = ServerSocket;
}

TCPSocket* Client::getConnectionWithServer() {
	try {
		// TODO
		string servAddress = udpSock->hostMap[serverHostname];
		TCPSocket* ServerSocket = new TCPSocket(servAddress, serverPort);
		return ServerSocket;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	return NULL;
}


void Client::sendCyphers(TCPSocket* sock) {
	
	//cout << "Sending cyphers to Server" << endl;

	// Our 2 buffers we want to send and their lengths
	cout << "  cipherToSendToServer1='" << cipherToSendToServer1 << "'" << endl;
	unsigned int firstTransLen = strlen(cipherToSendToServer1);
	char* firstTrans = new char [firstTransLen];
	memcpy(firstTrans, cipherToSendToServer1, firstTransLen);
	cout << "  cipherToSendToServer2='" << cipherToSendToServer2 << "'" << endl;
	unsigned int secondTransLen = strlen(cipherToSendToServer2);
	char* secondTrans = new char [secondTransLen];
	memcpy(secondTrans, cipherToSendToServer2, secondTransLen);


	try {
		// Send the length to the socket, and then the buffer, for both items
		sock->send(&firstTransLen, 4);
		sock->send(firstTrans, firstTransLen);
		sock->send(&secondTransLen, 4);
		sock->send(secondTrans, secondTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to B: (Step 3)" << endl;
	cout << "  first cypher  = '" << firstTrans << "'" << endl;
	cout << "  second cypher = '" << secondTrans << "'" << endl;
	cout << "  Ekb[Ks, Ida]" << endl;
	//cout << "sent stuff" << endl;
	delete (firstTrans);
	delete (secondTrans);
	
}


void Client::receiveNonce2(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
	int transCount = 1;
	char** recvBuff = new char*[transCount];
	
	try {
		uint64_t recvNonce;
		uint32_t nonceSize;
		sock->recv(&nonceSize,4);
		sock->recv(&recvNonce,nonceSize);
		cout << "Received from B: (Step 4)" << endl;
		cout << "  Eks[N2] = '" << recvNonce << "'" << endl;
		nonce2ReceivedFromServer = recvNonce;
		// change this after decryption done:
		cout << "  N2(decrypted) = '" << recvNonce << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	delete [] recvBuff;
}

void Client::sendMutatedNonce2(TCPSocket* sock) {
	
	encryptedAndMutatedNonceToSend = hashF(nonce2ReceivedFromServer);
		
	//cout << "Sending cyphers to peer" << endl;

	// Our 2 buffers we want to send and their lengths
	cout << "Sent to B: (Step 5)" << endl;
	cout << "  encryptedAndMutatedNonceToSend='" << encryptedAndMutatedNonceToSend << "'" << endl;
	
	// TODO: Encrypt this with Eks
	cout << "  mutatedNonce(encrypted)='" << encryptedAndMutatedNonceToSend << "'" << endl;
	unsigned int firstTransLen = 8;

	try {
		// Send the length to the socket, and then the buffer, for both items
		sock->send(&firstTransLen, 4);
		sock->send(&encryptedAndMutatedNonceToSend, firstTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "  Eks[f(N2)]" << endl;
	//cout << "sent stuff" << endl;
	
}

void Client::receiveOkay(TCPSocket* sock) {
	try {
		unsigned int s;					// Confirmation int sent by server (Should be 0)
		sock->recv(&s, 4);
		
		if (s) {
			cout << "Denied \"okay\" message by B! (Step 6)" << endl;
			exit(1);
		}
		cout << "Received \"okay\" message from B (Step 6)" << endl;
		
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}
	/*
	 *	
	 *	END AUTHENTICATION WITH SERVER
	 *	
	 */
	 
	/*
	 *	
	 *	BEGIN FTP
	 *	
	 */


