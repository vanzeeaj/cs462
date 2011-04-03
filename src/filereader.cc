#include <stdlib.h>
#include <stdio.h>
#include "filereader.h"

FileReader::FileReader(){
	file.open("config.cfg");
	readFile();
	file.close();
} 

void FileReader::readFile(){
	if (file.is_open()) {
		
		string output;
		getline(file,output);
		output = readLine();
		clientPort = atoi(output.c_str());
		output = readLine();
		clientHostname = getIP(output);
		output = readLine();
		clientNonce = atoll(output.c_str());
	
		getline(file,output);
		getline(file,output);
		output = readLine();
		parseKeys(output);		
		output = readLine();
		keys = new char [recvBuffSize];
		strcpy(keys, output.c_str());
		output = readLine();
		kdcPort = atoi(output.c_str());
		output = readLine();
		kdcHostname = getIP(output);	
		
		getline(file,output);
		getline(file,output);
		output = readLine();
		serverPort = atoi(output.c_str());
		output = readLine();
		serverHostname = getIP(output);
		output = readLine();
		serverNonce = atoll(output.c_str());
		
		getline(file,output);
		getline(file,output);
		output = readLine();
		packetLossRate = atof(output.c_str());
		output = readLine();
		ackLossRate = atof(output.c_str());
		output = readLine();
		corruptedPacketRate = atof(output.c_str());
		output = readLine();
		parseLongs(droppedPackets, &droppedPacketCount, output);
		output = readLine();
		parseLongs(droppedAcks, &droppedAckCount, output);
		output = readLine();
		parseLongs(corruptedPackets, &corruptedPacketCount, output);
		output = readLine();
		windowSize = atoll(output.c_str());
		output = readLine();
		packetSize = atoll(output.c_str());
		algorithm = readLine();
		fileToSend = readLine();
		
		
		
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
	clientKeyCount = 0;
	string tempLine = line;
	while( tempLine.size() > 0){
		commaLoc =  tempLine.find(',', 0);
		if(commaLoc == -1) commaLoc = tempLine.size() - 1;
		tempLine = tempLine.substr(commaLoc + 1,  tempLine.size() - commaLoc);
		clientKeyCount++;
	}
	
	clientKeys = new char*[clientKeyCount]; 
	
	for(int i = 0; i < clientKeyCount; i++){
		commaLoc = line.find(',', 0);
		clientKeys[i] =  new char[recvBuffSize];
		strcpy(clientKeys[i], line.substr(0, commaLoc).c_str());
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

string FileReader::getIP(string hostname){
	if (hostname.compare(0,5,"shiva") == 0) {
		return "137.28.8.143";
	} else if (hostname.compare(0,5,"clark") == 0) {
		return  "137.28.8.160";
	} else if (hostname.compare(0,4,"andy") == 0) {
		return "137.28.8.161";
	} else {
		cout << "Hostname " << hostname << " could not be resolved... exiting." << endl;
		exit(1);
	}
}
