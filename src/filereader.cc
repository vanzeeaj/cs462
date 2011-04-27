#include <stdlib.h>
#include <stdio.h>
#include "filereader.h"

FileReader::FileReader(){
	file.open("config.cfg");
	readFile();
	file.close();
} 


//TODO ADD CRC NUMBER for shared.

void FileReader::readFile(){
	if (file.is_open()) {
		
		//CLIENT INFORMATION
		string output;
		getline(file,output);
		output = readLine();
		clientInfo.clientPort = atoi(output.c_str());
		output = readLine();
		clientInfo.clientIP = getIP(output);
		output = readLine();
		clientInfo.nonce = atoll(output.c_str());
	
		cout << "here1" << endl;
	
		//KDC info
		getline(file,output);
		getline(file,output);
		output = readLine();
		parseKeys(output);		
		clientInfo.keyA = new char[recvBuffSize];
		strcpy(clientInfo.keyA, KDCInfo.clientServerKeys[0]);
		serverInfo.keyB = new char[recvBuffSize];
		strcpy(serverInfo.keyB, KDCInfo.clientServerKeys[1]);
		output = readLine();
		KDCInfo.sessionKey = new char [recvBuffSize];
		strcpy(KDCInfo.sessionKey, output.c_str());
		output = readLine();
		clientInfo.kdcPort = atoi(output.c_str());
		KDCInfo.kdcPort = clientInfo.kdcPort;
		output = readLine();
		clientInfo.kdcIP = getIP(output);	
		
		cout << "here2" << endl;

		
		//Server info
		getline(file,output);
		getline(file,output);
		output = readLine();
		clientInfo.serverPort = atoi(output.c_str());
		serverInfo.serverPort = clientInfo.serverPort;
		output = readLine();
		clientInfo.serverIP = getIP(output);
		serverInfo.serverIP = new char[recvBuffSize];
		strcpy(serverInfo.serverIP, clientInfo.serverIP);
		output = readLine();
		serverInfo.nonce = atoll(output.c_str());
		
		
		cout << "here3" << endl;

		
		//Misc info
		getline(file,output);
		getline(file,output);
		output = readLine();
		clientInfo.packetLossRate = atof(output.c_str());
		output = readLine();
		sharedInfo.ackLossRate = atof(output.c_str());
		output = readLine();
		clientInfo.corruptedPacketRate = atof(output.c_str());
		output = readLine();
		parseLongs(clientInfo.droppedPackets, &(clientInfo.droppedPacketCount), output);
		output = readLine();
		parseLongs(sharedInfo.droppedAcks, &(sharedInfo.droppedAckCount), output);
		output = readLine();
		parseLongs(clientInfo.corruptedPackets, &(clientInfo.corruptedPacketCount), output);
		output = readLine();
		sharedInfo.windowSize = atoll(output.c_str());
		output = readLine();
		sharedInfo.packetSize = atoll(output.c_str());
		clientInfo.algorithm = new char[recvBuffSize];
		strcpy(clientInfo.algorithm, readLine().c_str());
		sharedInfo.fileToSend = new char[recvBuffSize];
		strcpy(sharedInfo.fileToSend, readLine().c_str());
		output = readLine();
		sharedInfo.CRCcode = atoll(output.c_str());
		
		cout << "here4" << endl;

		
	} else {
		cerr << "Couldn't open 'config.cfg'... exiting" << endl;
		exit(1);
	}
}

string FileReader::readLine() {
	string output;
	getline(file,output);	
	//cout << "output size is:" << output.size() << ",= is at:" << output.find('=',0) << endl;
	//cout << output.substr(output.find('=',0)+1, output.size()-output.find('=',0)-1) << endl;
	return output.substr(output.find('=',0)+1, output.size()-output.find('=',0)-1);	
}

void FileReader::parseKeys(string line){
	int commaLoc = 0;	
	KDCInfo.clientCount = 0;
	string tempLine = line;
	while( tempLine.size() > 0){
		commaLoc =  tempLine.find(',', 0);
		if(commaLoc == -1) commaLoc = tempLine.size() - 1;
		tempLine = tempLine.substr(commaLoc + 1,  tempLine.size() - commaLoc);
		KDCInfo.clientCount++;
	}
	
	KDCInfo.clientServerKeys = new char*[KDCInfo.clientCount]; 
	
	for(int i = 0; i < KDCInfo.clientCount; i++){
		commaLoc = line.find(',', 0);
		KDCInfo.clientServerKeys[i] =  new char[recvBuffSize];
		strcpy(KDCInfo.clientServerKeys[i], line.substr(0, commaLoc).c_str());
		line = line.substr(commaLoc + 1, line.size() - commaLoc);
	}
}

void FileReader::parseLongs(uint64_t* data, int* count, string line){
	int commaLoc = 0;
	*count = 0;
	string tempLine = line;
	while( tempLine.size() > 0){
		commaLoc =  tempLine.find(',', 0);
		if(commaLoc == -1) commaLoc = tempLine.size() - 1;
		tempLine = tempLine.substr(commaLoc + 1,  tempLine.size() - commaLoc);
		*count++;
	}
	
	data = new uint64_t [*count]; 
	
	for(int i = 0; i < *count; i++){
		commaLoc = line.find(',', 0);
		string temp = line.substr(0, commaLoc);
		data[i] =  atoll(temp.c_str());
		line = line.substr(commaLoc + 1, line.size() - commaLoc);
	}
}

char* FileReader::getIP(string hostname){
	char* temp = new char[recvBuffSize];
	
	if (hostname.compare(0,5,"shiva") == 0) {
		temp = "137.28.8.143";
	} else if (hostname.compare(0,5,"clark") == 0) {
		temp =  "137.28.8.160";
	} else if (hostname.compare(0,4,"andy") == 0) {
		temp = "137.28.8.161";
	} else {
		cout << "Hostname " << hostname << " could not be resolved... exiting." << endl;
		exit(1);
	}
	return temp;
}
