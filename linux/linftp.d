blowfish.o: blowfish.cc blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.cc \
  ByteReorderingFunctions.h
client.o: client.cc client.h blowfish.h PracticalSocket.h \
  ByteReorderingFunctions.h
EncryptedSockets.o: EncryptedSockets.cc
kdc.o: kdc.cc
main.o: main.cc
PracticalSocket.o: PracticalSocket.cc PracticalSocket.h
server.o: server.cc
blowfish.o: blowfish.cc blowfish.h
blowfish.o: blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.h
client.o: client.h blowfish.h PracticalSocket.h ByteReorderingFunctions.h
EncryptedSockets.o: EncryptedSockets.h PracticalSocket.h
kdc.o: kdc.h
PracticalSocket.o: PracticalSocket.h
server.o: server.h
