#include <iostream>
#include "common.h"
#include "server.h"
#include "kdc.h"
#include "client.h"

using namespace std;

int main(int argc, char** argv){
	KDC* k;
	Client* c;
	Server* s;
	
	if (*argv[1] == 'k') {
		cout << "making a kdc" << endl;
		char** nck = new char*[2]();
		nck[0] = argv[3];
		nck[1] = argv[4];
		k = new KDC(argv[2], 2, nck, atoi(argv[5]), argv[6], atoi(argv[7]));
		k->execute();
	} else if (*argv[1] == 's') {
		cout << "making a server" << endl;
		s = new Server(atoi(argv[2]), argv[3], argv[4], atol(argv[5]));
		s->listenForCommunication();
	} else if (*argv[1] == 'c') {
		cout << "making a client" << endl;
		c = new Client(argv[2], atoi(argv[3]),argv[4],atoi(argv[5]),
				argv[6],atoi(argv[7]),atol(argv[8]),argv[9]);
		c->initiate();
	}
	
	return 0;
}

