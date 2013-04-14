include ../../../makefile.conf

CONNLIB = libdatafileconn.so
PFIELD_NAME = DataFileReaderPField

OBJS = $(PFIELD_NAME).o glue.o
SRCS = $(PFIELD_NAME).cpp glue.cpp

CXXFLAGS += -I. -I../../rtcmix

all: $(CONNLIB)

$(CONNLIB): depend $(OBJS)
	$(CXX) $(SHARED_LDFLAGS) -o $@ $(OBJS)

glue.o: depend glue.cpp
	$(CXX) $(CXXFLAGS) -c -o glue.o glue.cpp

depend:
	-$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $(SRCS)' > depend

-include depend

install: all
	$(INSTALL) $(CONNLIB) $(LIBDESTDIR)

uninstall:
	$(RM) $(LIBDESTDIR)/$(CONNLIB)

clean:
	$(RM) *.o $(CONNLIB)

cleanall: clean
	$(RM) depend

