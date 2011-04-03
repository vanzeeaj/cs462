#include <iostream>
#include "common.h"
#include "server.h"
#include "kdc.h"
#include "client.h"
#include "filereader.h"

using namespace std;

void testmain(int, char**);

int main(int argc, char** argv){

	// testmain(argc, argv);
	KDC* k;
	Client* c;
	Server* s;
	FileReader* f = new FileReader();

	// Server on clark, KDC on clark, client on andy
	if (*argv[1] == 'k') {
		cout << "making a kdc" << endl;
		
		k = new KDC(f->keys, f->clientKeyCount, f->clientKeys, f->kdcPort);
		k->execute();
		
	} else if (*argv[1] == 's') {
		cout << "making a server" << endl;
		s = new Server(f->serverPort, f->serverHostname, f->clientKeys[1], f->serverNonce); 
		s->listenForCommunication();
		
	} else if (*argv[1] == 'c') {
		cout << "making a client" << endl;
		c = new Client(f->kdcHostname, f->kdcPort, f->clientHostname, f->clientPort, 
						f->serverHostname, f->serverPort, f->clientNonce, f->clientKeys[0], 
						f->fileToSend, f->packetSize);
		c->initiate();
	}
	
	return 0;
}

void testmain(int argc, char** argv){
	if (argc == 2){
		cout << "client being created" << endl;
		EncryptedTCPSocket* tsock = new EncryptedTCPSocket("137.28.8.160",34512);
		char* thepass = new char[16];
		thepass = "asdf1235sddf%\0";
		tsock->bf->Set_Passwd(thepass);
		cout << "pass set" << endl;
		
		char* ourPacket = new char [256];
		int packetLen = 256;
		strncpy(ourPacket,"1234567890123456789012345678901234567890123456789012345678901234567890",256);
		string str = ourPacket;
		cout << "sending packet over with contents: " << str << endl;
		tsock->sendPayload(ourPacket, packetLen);
		cout << "payload sent" << endl;
	} else {
		cout << "server being created, waiting..." << endl;
		EncryptedTCPServerSocket* tssock = new EncryptedTCPServerSocket("137.28.8.160",34512);
		EncryptedTCPSocket* tsock = tssock->accept();		// blocking
		cout << "connection initiated" << endl;
		
		char* buffer = new char[256];
		int bufferLen = 256;
		char* thepass = new char[16];
		thepass = "asdf1235sddf%\0";
		tsock->bf->Set_Passwd(thepass);
		tsock->recvPayload(buffer, bufferLen);
		string str = buffer;
		cout << "received: " << str << endl;
	}
	
}
