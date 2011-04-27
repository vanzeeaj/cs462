blowfish.o: blowfish.cc blowfish.h
client.o: client.cc client.h EncryptedSockets.h PracticalSocket.h \
 common.h blowfish.h Encrypter.h
EncryptedSockets.o: EncryptedSockets.cc EncryptedSockets.h \
 PracticalSocket.h common.h blowfish.h
Encrypter.o: Encrypter.cc Encrypter.h PracticalSocket.h common.h \
 blowfish.h
filereader.o: filereader.cc filereader.h common.h
kdc.o: kdc.cc kdc.h EncryptedSockets.h PracticalSocket.h common.h \
 blowfish.h Encrypter.h
main.o: main.cc common.h server.h EncryptedSockets.h PracticalSocket.h \
 blowfish.h Encrypter.h kdc.h client.h filereader.h
PracticalSocket.o: PracticalSocket.cc PracticalSocket.h common.h
server.o: server.cc server.h common.h EncryptedSockets.h \
 PracticalSocket.h blowfish.h Encrypter.h
blowfish.o: blowfish.cc blowfish.h
blowfish.o: blowfish.h
client.o: client.h EncryptedSockets.h PracticalSocket.h common.h \
 blowfish.h Encrypter.h
common.o: common.h
EncryptedSockets.o: EncryptedSockets.h PracticalSocket.h common.h \
 blowfish.h
Encrypter.o: Encrypter.h PracticalSocket.h common.h blowfish.h
filereader.o: filereader.h common.h
kdc.o: kdc.h EncryptedSockets.h PracticalSocket.h common.h blowfish.h \
 Encrypter.h
PracticalSocket.o: PracticalSocket.h common.h
server.o: server.h common.h EncryptedSockets.h PracticalSocket.h \
 blowfish.h Encrypter.h
