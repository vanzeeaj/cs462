#include <iostream>
#include <stdlib.h>
#include "client.h"

using namespace std;

int main(int argc, char *argv[]) {
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
		Client* c = new Client(jj, k, b, i);
		//c->listenForTraffic();
		//c->greg_execute();
	}
	return 0;
}
