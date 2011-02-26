#include "server.h"

using namespace std;

Server::Server(int serverPort, char* serverHostname, char* keyB, uint64_t nonce){
	this->serverPort = serverPort;
	this->keyB = keyB;
	this->nonce = nonce;
	this->serverHostname = serverHostname;
	//TODO 
	//Here we need to shell out to get our IP.
}


uint64_t Server::hashF(uint64_t nonce) {
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

void Server::listenForCommunication() {
	//cout << "Server initiated " << endl;
	try {
		theServerSocket = new TCPServerSocket(serverPort);
		
		// Accept incoming connections (blocking)
		clientSocket = theServerSocket->accept();
		
		// Connection initiated, time to do things.
		cout << "Client connected..." << endl;
		
		receiveCyphers(clientSocket);
		sendNonce2(clientSocket);
		receiveMutatedNonce2(clientSocket);
		sendOkay(clientSocket);
		
		cout << "Finished giving authentication to client" << endl;
	
		activeSocket = clientSocket;
	
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}

void Server::receiveCyphers(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	unsigned int f;
	sock->recv(&f, 4);
	//cout << "received len:" << f << endl;
	cipherToReceiveFromPeer1 = new char [f];
	sock->recv(cipherToReceiveFromPeer1, f);
	cout << "cipherToReceiveFromPeer1='" << cipherToReceiveFromPeer1 << "'" << endl;
		
	// Same as above, except for second transmission
	unsigned int s;
	sock->recv(&s, 4);
	//cout << "received len:" << s << endl;
	cipherToReceiveFromPeer2 = new char [s];
	sock->recv(cipherToReceiveFromPeer2, s);
	cout << "cipherToReceiveFromPeer2='" << cipherToReceiveFromPeer2 << "'" << endl;	
	cout << "Received from A: (Step 3)" << endl;
	cout << "  EKb[Ks, Ida]" << endl;
	
	// TODO: Decrypt	
	sessionKey = cipherToReceiveFromPeer1;
	
	// Change this next line after decryption is done:
	cout << "  Ks(decrypted) = '" << sessionKey << "'" << endl;
	cout << "  Ida = '" << cipherToReceiveFromPeer2 << "'" << endl;

	//cout << "finished recv" << endl;

}

void Server::sendNonce2(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint32_t nonceSize = 8;
	uint64_t cipherNonce = nonce;
	uint32_t cipher1Size = 8;
	//cout << "Everything initialized, starting sends..." << endl;

	try {
		// 5 buffers being sent in total.  Reordering the sizes so
		// the goonies can understand, then sending them, then
		// sending the buffers.
		// SEND 1
		// TODO ENCRYPT WITH Ks
		sock->send(&nonceSize, 4);
		sock->send(&cipherNonce, cipher1Size);
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 4)" << endl;
	cout << "  N2 = '" << nonce << "'" << endl;
	cout << "  Eks[N2] = '" << cipherNonce << endl;
}

void Server::receiveMutatedNonce2(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	unsigned int f;
	sock->recv(&f, 4);
	//cout << "received len:" << f << endl;
	uint64_t mutatedNonceToReceive;
	sock->recv(&mutatedNonceToReceive, f);
	cout << "mutatedNonceToReceive='" << mutatedNonceToReceive << "'" << endl;
	cout << "Received from A: (Step 5)" << endl;
	cout << "  Eks[f(N2)]" << endl;
	// TODO: Decrypt
	cout << "  f(N2) (decrypted) = '" << mutatedNonceToReceive << "'" << endl;	
	//cout << "finished recv" << endl;
	uint64_t tempnonce = hashF(nonce);
	
	if (mutatedNonceToReceive != tempnonce) {
		cout << "Received f(N2) != local f(N2).  Exiting..." << endl;
		exit(1);
	}

}

void Server::sendOkay(TCPSocket* sock) {
	cout << "Sending \"okay\" message to A (Step 6)" << endl;
	int k = 0;
	sock->send(&k, 4);
}


