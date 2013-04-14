include ../../../makefile.conf

OBJS = RTcmixWindow.o
SRCS = RTcmixWindow.cpp
#CXXFLAGS += -I. -I../../rtcmix

all: depend $(OBJS)

#	$(CXX) $(CXXFLAGS) -c -o glue.o glue.cpp

depend:
	-$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $(SRCS)' > depend

-include depend

install: all

uninstall:

clean:
	$(RM) *.o

cleanall: clean
	$(RM) depend

