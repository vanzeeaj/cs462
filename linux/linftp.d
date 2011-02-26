blowfish.o: blowfish.cc blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.cc
client.o: client.cc client.h EncryptedSockets.h PracticalSocket.h \
  blowfish.h common.h
EncryptedSockets.o: EncryptedSockets.cc EncryptedSockets.h \
  PracticalSocket.h blowfish.h
kdc.o: kdc.cc kdc.h EncryptedSockets.h PracticalSocket.h blowfish.h \
  common.h
main.o: main.cc common.h server.h kdc.h EncryptedSockets.h \
  PracticalSocket.h blowfish.h client.h
PracticalSocket.o: PracticalSocket.cc PracticalSocket.h
server.o: server.cc server.h
blowfish.o: blowfish.cc blowfish.h
blowfish.o: blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.h
client.o: client.h EncryptedSockets.h PracticalSocket.h blowfish.h \
  common.h
common.o: common.h
EncryptedSockets.o: EncryptedSockets.h PracticalSocket.h blowfish.h
kdc.o: kdc.h EncryptedSockets.h PracticalSocket.h blowfish.h
PracticalSocket.o: PracticalSocket.h
server.o: server.h
