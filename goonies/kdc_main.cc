#include <iostream>
#include "kdc.h"
#include "common.h"

using namespace std;

int main(int argc, char *argv[]) {
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

