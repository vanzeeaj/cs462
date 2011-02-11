PracticalSocket.o: PracticalSocket.cpp PracticalSocket.h
blowfish.o: blowfish.cc blowfish.h
ByteReorderingFunctions.o: ByteReorderingFunctions.cc \
  ByteReorderingFunctions.h
kdc.o: kdc.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h common.h \
  blowfish.h
kdc_main.o: kdc_main.cc kdc.h PracticalSocket.h ByteReorderingFunctions.h \
  common.h
