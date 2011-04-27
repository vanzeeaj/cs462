#include "server.h"

using namespace std;

//Server::Server(int serverPort, string serverHostname, char* keyB, uint64_t nonce){
Server::Server(server_info* info){
	this->serverPort = info->serverPort;
	this->keyB = info->keyB;
	this->nonce = info->nonce;
	this->serverHostname = info->serverIP;
	e = new Encrypter(keyB);
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
		
		cout << "Starting FTP" << endl;
		startFTP();
	
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}

void Server::receiveCyphers(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	uint64_t f;
	sock->recv(&f, 8);
	f = *(uint64_t*)e->decrypt(&f, 8);
	e->htonll(&f);
	cout << "received len:" << f << "keyb = " << keyB << endl;
	cipherToReceiveFromPeer1 = new char [f];
	sock->recv(cipherToReceiveFromPeer1, f);
	cout << "cipherToReceiveFromPeer1='" << cipherToReceiveFromPeer1 << "'" << endl;
		
	// Same as above, except for second transmission
	uint64_t s;
	sock->recv(&s, 8);
	s = *(uint64_t*)e->decrypt(&s, 8);
	e->htonll(&s);
	cout << "received len:" << s << endl;
	cipherToReceiveFromPeer2 = new char [s];
	sock->recv(cipherToReceiveFromPeer2, s);
	cout << "cipherToReceiveFromPeer2='" << cipherToReceiveFromPeer2 << "'" << endl;	
	cout << "Received from A: (Step 3)" << endl;
	cout << "  EKb[Ks, Ida]" << endl;
	
	sessionKey = e->decrypt(cipherToReceiveFromPeer1, f);
	clientID = e->decrypt(cipherToReceiveFromPeer2, s);
	
	// Change this next line after decryption is done:
	cout << "  Ks(decrypted) = '" << sessionKey << "'" << endl;
	cout << "  clientID(decrypted) = '" << clientID << "'" << endl;

	e->changeKey(sessionKey);
}

void Server::sendNonce2(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint64_t nonceSize = 8;
	char* encNonceSize;
	char* encNonce;
	//cout << "Everything initialized, starting sends..." << endl;

	try {
		// SEND 1
		e->htonll(&nonceSize);
		encNonceSize = e->encrypt(&nonceSize, 8);		
		sock->send(encNonceSize, 8);
		e->htonll(&nonceSize);
		
		e->htonll(&nonce);
		encNonce = e->encrypt(&nonce, nonceSize);
		sock->send(encNonce, e->getEncryptedLength(nonceSize));
		e->htonll(&nonce);
		
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 4)" << endl;
	cout << "  N2 = '" << nonce << "'" << endl;
	//cout << "  Eks[N2] = '" << encNonce << endl;
}

void Server::receiveMutatedNonce2(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	uint64_t f;
	sock->recv(&f, 8);
	f = *((uint64_t*)e->decrypt(&f, 8));
	e->htonll(&f);
	cout << "received len:" << f << endl;
	uint64_t mutatedNonceToReceive;
	sock->recv(&mutatedNonceToReceive, f);	
	//cout << "mutatedNonceToReceive='" << mutatedNonceToReceive << "'" << endl;
	cout << "Received from A: (Step 5)" << endl;
	cout << "  Eks[f(N2)]" << endl;
	
	
	cout << "  f(N2) (encrypted) = '" << mutatedNonceToReceive << "'" << endl;	
	mutatedNonceToReceive = *((uint64_t*)e->decrypt(&mutatedNonceToReceive, f));
	e->htonll(&mutatedNonceToReceive);
	cout << "  f(N2) (decrypted) = '" << mutatedNonceToReceive << "'" << endl;	
	//cout << "finished recv" << endl;
	uint64_t tempnonce = hashF(nonce);
	
	
	
	if (mutatedNonceToReceive != tempnonce) {
		cout << "Received f(N2) != local f(N2).  Exiting..." << endl;
		exit(1);
	}

}

void Server::sendOkay(TCPSocket* sock) {

	//TODO add encryption w/ Ks
	cout << "Sending \"okay\" message to A (Step 6)" << endl;
	uint64_t k = 0;
	e->htonll(&k);
	k = *(uint64_t*)e->encrypt(&k, 8);
	sock->send(&k, 8);
}

void Server::startFTP(){
	cout << "Initializing UDPSocket" << endl;
	initUDPSocket();
	cout << "Starting receive" << endl;
	beginRecv();
	cout << "Finished Receiving... closing socket and killing thread." << endl;
	closeUDPSocketAndKillThread();
}

void Server::initUDPSocket(){
	
}

void Server::beginRecv(){
	
}

void Server::closeUDPSocketAndKillThread(){
	
}

