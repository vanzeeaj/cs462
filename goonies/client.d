PracticalSocket.o: PracticalSocket.cpp PracticalSocket.h
blowfish.o: blowfish.cc blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.cc \
  ByteReorderingFunctions.h
client.o: client.cc client.h blowfish.h PracticalSocket.h \
  ByteReorderingFunctions.h common.h
client_main.o: client_main.cc client.h blowfish.h PracticalSocket.h \
  ByteReorderingFunctions.h
