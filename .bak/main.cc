#include <iostream>
#include <stdlib.h>
#include "kdc.h"
#include "server.h"
#include "client.h"
#include "common.h"

using namespace std;

int main(int, char**);
int kdc_main(int, char**);
int server_main(int, char**);
int client_main(int, char**);
void dieAndShowUsage();

int main(int argc, char** argv) {
	if (argv[1] == "kdc") {
		kdc_main(argc, argv);
	} else if (argv[1] == "client") {
		client_main(argc, argv);
	} else if (argv[1] == "server") {
		server_main(argc, argv);
	} else {
		cout << argv[1] << " undefined. See usage." << endl;
		return 1;
	}
	return 0;
}

int kdc_main(int argc, char *argv[]) {
	if (argc < 4) {
		dieAndShowUsage();
	}
	
	cout << "KDC initiated" << endl;
	// Get session key from command line
	//char* sessionKey = argv[1];
	char* sessionKey = new char[16];
	int j;
	for (j = 0; j < 16; j++) {
		sessionKey[j] = '0';
	}
	for (j = 0; j < 16; j++) {
		if (argv[1][j] == '\0') { break; }
		else { sessionKey[j] = argv[1][j]; }
	}
	
	cout << "sessionKey = " << sessionKey << endl;
	
	// Get client keys from command line
	int clientCount = argc - 2;
	char** clientKeys = new char*[clientCount];
	int i;
	for (i = 0; i < clientCount; i++) {
		//clientKeys[i] = argv[i + 2];
		clientKeys[i] = new char[16];
		for (j = 0; j < 16; j++) {
			clientKeys[i][j] = '0';
		}
		for (j = 0; j < 16; j++) {
			if (argv[1][j] == '\0') { break; }
			else { clientKeys[i][j] = argv[i+2][j]; }
		}
		cout << "clientKeys[" << i << "] = " << clientKeys[i] << endl;
	}
	
	// Create KDC
	KDC* kdc = new KDC(sessionKey, clientCount, clientKeys);
	
	
	// Run the KDC
	kdc->execute();
	
	// Cleanup
	delete kdc;
	delete[] clientKeys;
	return 0;
}

int client_main(int argc, char *argv[]) {
	if (argc < 4) {
		cout << "usage: ./client \"size\" \"nonce\" \"key\" \"initiator\"" << endl;
	} else {
		long i = atoi(argv[1]);
		//char* jj = argv[2];
		uint64_t jj = (uint64_t)atoll(argv[2]);
		
		
		//char* k = argv[3];
		char* k = new char[16];
		int j;
		for (j = 0; j < 16; j++) {
			k[j] = '0';
		}
		for (j = 0; j < 16; j++) {
			if (argv[3][j] == '\0') { break; }
			else { k[j] = argv[3][j]; }
		}
		
		int l = atoi(argv[4]);
		bool b = (l ? true : false);
		cout << "Client ";
		if (b) cout << "(A) (initiator) ";
		else cout << "(B) (non-initiator) ";
		cout << "started with info(nonce:" << jj << ", key:" << k << ", i:" << b << ", size:" << i << ")" << endl;
		new Client(jj, k, b, i);
	}
	return 0;
}

int server_main(int argc, char** argv){

	return 0;
}

void dieAndShowUsage() {
	cerr << "Usage:" << endl;
	cerr << "./kdc Ks Ka Kb [Kc [Kd ...]]" << endl;
	cerr << "Ks = Session key" << endl;
	cerr << "Ka = Encryption key for Client A" << endl;
	cerr << "Kb = Encryption key for Client B" << endl;
	cerr << "Kc = Encryption key for Client C (if there is a third client)" << endl;
	cerr << "Kd = Encryption key for Client D (if there is a fourth client)" << endl;
	exit(1);
}

