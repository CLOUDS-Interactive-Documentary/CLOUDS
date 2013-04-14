include ../../../makefile.conf

CONNLIB = libpfbusconn.so

OBJS = PFBusPField.o pfbusglue.o
SRCS = PFBusPField.cpp pfbusglue.cpp

CXXFLAGS += -I. -I../../rtcmix

all: $(CONNLIB)

$(CONNLIB): depend $(OBJS)
	$(CXX) $(SHARED_LDFLAGS) -o $@ $(OBJS)

pfbusglue.o: pfbusglue.cpp PFBusPField.h
	$(CXX) $(CXXFLAGS) -c -o pfbusglue.o pfbusglue.cpp

depend:
	-$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $(SRCS)' > depend

-include depend

install: all
	$(INSTALL) $(CONNLIB) $(LIBDESTDIR)

uninstall:
	$(RM) $(LIBDESTDIR)/$(CONNLIB)

clean:
	$(RM) *.o $(LIBDESTDIR)/$(CONNLIB) $(CONNLIB)

cleanall: clean
	$(RM) depend

