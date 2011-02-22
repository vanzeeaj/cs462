blowfish.o: blowfish.cc blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.cc \
  ByteReorderingFunctions.h
client.o: client.cc client.h blowfish.h PracticalSocket.h \
  ByteReorderingFunctions.h common.h
client_main.o: client_main.cc client.h blowfish.h PracticalSocket.h \
  ByteReorderingFunctions.h
kdc.o: kdc.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h common.h \
  blowfish.h
kdc_main.o: kdc_main.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h \
  common.h
main.o: main.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h \
  server.h client.h blowfish.h common.h
PracticalSocket.o: PracticalSocket.cc PracticalSocket.h
server.o: server.cc
server_main.o: server_main.cc
