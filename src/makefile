####################################
#MAKEFILE FOR NETWORKS SFTP PROJECT#
####################################

### Check Architecture ###
ARCH := $(shell perl ./.testEndian.pl)
ifeq ($(ARCH),solaris)
BUILDDIR=../shiva/
EXEC=$(BUILDDIR)sunftp
FLAGS=-lrt -lsocket -lnsl -lpthread
endif

ifeq ($(ARCH),linux)
BUILDDIR=../linux/
EXEC=$(BUILDDIR)linftp
FLAGS=-lrt
endif


### Generic Make ###
CCFILES := $(shell ls | grep .cc)
_OFILES=$(CCFILES:.cc=.o)
OFILES= $(patsubst %,$(BUILDDIR)%,$(_OFILES))
DEPS := $(shell ls | grep .h)

#The main linking rule
all: $(EXEC)

$(BUILDDIR)%.o: %.cc $(DEPS)
	g++ -c -o $@ $<

$(EXEC): $(OFILES)
	g++ -o $@ $^ $(FLAGS)

.PHONY: clean

clean: 
	rm -f *~
	rm -f *.o
	rm -f $(BUILDDIR)*.o
	rm -f $(EXEC)
	g++ $(CCFILES) $(DEPS) -MM > $(EXEC).d

fullclean:
	rm -f *~
	rm -f *.o
	rm -f ../linux/*.o
	rm -f ../shiva/*.o
	rm -f ../shiva/sunftp
	rm -f ../linux/linftp
	g++ $(CCFILES) $(DEPS) -MM > $(EXEC).d

#Always include the dependency file unless we run the clean command
#which will make it even if it does not exist
ifneq ($(MAKECMDGOALS), clean)
include $(BUILDDIR)$(EXEC).d
endif
