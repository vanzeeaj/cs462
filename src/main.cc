#include <iostream>
#include "common.h"
#include "server.h"
#include "kdc.h"
#include "client.h"

using namespace std;

void testmain(int, char**);

int main(int argc, char** argv){

	// testmain(argc, argv);
	KDC* k;
	Client* c;
	Server* s;


	// Server on clark, KDC on clark, client on andy
	if (*argv[1] == 'k') {
		cout << "making a kdc" << endl;
		char** nck = new char*[2]();
		nck[0] = "12345\0";
		nck[1] = "67890\0";
		
		k = new KDC("22222\0", 2, nck, 21220);
		
		//char** nck = new char*[2]();
		//nck[0] = argv[3];
		//nck[1] = argv[4];
		//k = new KDC(argv[2], 2, nck, atoi(argv[5]), argv[6], atoi(argv[7]));
		k->execute();
		
		
	} else if (*argv[1] == 's') {
		cout << "making a server" << endl;
		
		s = new Server(21212, "67890\0", "clark\0", 987654321); 
		//s = new Server(atoi(argv[2]), argv[3], argv[4], atol(argv[5]));
		
		s->listenForCommunication();
		
	} else if (*argv[1] == 'c') {
		cout << "making a client" << endl;
		
		c = new Client("clark\0", 21220, "andy\0", 21211, "clark\0", 21212, 123456789, "12345\0");
		//c = new Client(argv[2], atoi(argv[3]),argv[4],atoi(argv[5]),
		//		argv[6],atoi(argv[7]),atol(argv[8]),argv[9]);
		
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
