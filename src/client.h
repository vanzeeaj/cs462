#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <queue>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include "EncryptedSockets.h"


class Client {
	public:
		Client();
		~Client();
	
	private:
		int listeningPort;
		int sendingPort;

};

#endif
