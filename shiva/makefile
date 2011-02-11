# Makefile for our encrypted, cross platform, socketed sieve

goonies:	client 
shiva:	kdc

client:	client.cc blowfish.cc PracticalSocket.cpp client_main.cc networkByteReordering.cc
	g++ -o client client.cc blowfish.cc PracticalSocket.cpp client_main.cc networkByteReordering.cc

kdc:	kdc.cc blowfish.cc kdc_main.cc PracticalSocket.cpp networkByteReordering.cc
	g++ -o kdc kdc.cc blowfish.cc kdc_main.cc PracticalSocket.cpp networkByteReordering.cc -lsocket -lnsl -lpthread

clean:
	rm -f *~
	rm -f *.o
	rm -f kdc
	rm -f client
	rm -f kdc-lin

