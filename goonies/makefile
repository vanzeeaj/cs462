#MAKEFILE FOR NETWORKS SFTP PROJECT
CCFILES=PracticalSocket.cpp blowfish.cc ByteReorderingFunctions.cc #cc files used in everything
FLAGS=-O2	#flags used in everything

#Stuff specific to executable one
CCFILES1=client.cc client_main.cc
EXEC1=client

#Stuff specific to executable two
CCFILES2=server.cc server_main.cc
EXEC2=server

#Stuff specific to executable three
CCFILES3=kdc.cc kdc_main.cc
EXEC3=kdc
FLAGS3=-pthread -lrt

#The included dependency file contains all the incremental compilation info,
#however we use the implicit rule for making each one which is (simplified): 
#g++ $(CXXFLAGS) -c -o Foo.o Foo.cc
#Thus we place our complier flags into this default variable
CXXFLAGS=-Wall

#Create all the needed .o file names from the .cc files
OFILES=$(CCFILES:.cc=.o)
OFILES1=$(CCFILES1:.cc=.o)
OFILES2=$(CCFILES2:.cc=.o)
OFILES3=$(CCFILES3:.cc=.o)

#The main linking rule
all: $(EXEC1) $(EXEC2) $(EXEC3)
	
$(EXEC1): $(OFILES) $(OFILES1)
	g++ -o $(EXEC1) $(OFILES) $(OFILES1) $(FLAGS) $(FLAGS1)
$(EXEC2): $(OFILES) $(OFILES2)
	g++ -o $(EXEC2) $(OFILES) $(OFILES2) $(FLAGS) $(FLAGS2)
$(EXEC3): $(OFILES) $(OFILES3)
	g++ -o $(EXEC3) $(OFILES) $(OFILES3) $(FLAGS) $(FLAGS3)

#Clean not only cleans up object files, but also recreates the dependency file
clean: 
	rm -f *~
	rm -f *.o
	rm -f $(EXEC)
	rm -f $(EXEC1)
	rm -f $(EXEC2)
	rm -f $(EXEC3)
	g++ $(CCFILES) $(CCFILES1) -MM > $(EXEC1).d
	g++ $(CCFILES) $(CCFILES2) -MM > $(EXEC2).d
	g++ $(CCFILES) $(CCFILES3) -MM > $(EXEC3).d

#Always include the dependency file unless we run the clean command
#which will make it even if it does not exist
ifneq ($(MAKECMDGOALS), clean)
include $(EXEC1).d
include $(EXEC2).d
include $(EXEC3).d
endif
