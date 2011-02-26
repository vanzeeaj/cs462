#ifndef SERVER_H
#define SERVER_H

using namespace std;

class Server {
	public:
		Server();
		~Server();
	
	private:
		int listeningPort;
		int sendingPort;
		
};

#endif
