blowfish.o: blowfish.cc blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.cc \
  ByteReorderingFunctions.h
client.o: client.cc client.h blowfish.h PracticalSocket.h \
  ByteReorderingFunctions.h common.h
kdc.o: kdc.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h common.h \
  blowfish.h
main.o: main.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h \
  server.h client.h blowfish.h common.h
PracticalSocket.o: PracticalSocket.cc PracticalSocket.h
server.o: server.cc
blowfish.o: blowfish.cc blowfish.h
blowfish.o: blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.h
client.o: client.h blowfish.h PracticalSocket.h ByteReorderingFunctions.h
common.o: common.h
kdc.o: kdc.h PracticalSocket.h ByteReorderingFunctions.h
PracticalSocket.o: PracticalSocket.h
server.o: server.h
