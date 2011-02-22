PracticalSocket.o: PracticalSocket.cc PracticalSocket.h
blowfish.o: blowfish.cc blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.cc \
  ByteReorderingFunctions.h
main.o: main.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h \
  server.h client.h blowfish.h common.h
