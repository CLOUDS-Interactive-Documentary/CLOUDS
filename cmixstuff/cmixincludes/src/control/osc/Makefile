include ../../../makefile.conf

OBJS = RTcmixOSC.o RTOscPField.o glue.o
SRCS = RTcmixOSC.cpp RTOscPField.cpp glue.cpp

CXXFLAGS += -I. -I../../rtcmix $(LIBLO_CFLAGS)

LIBOSC = liboscconn.so

all: $(LIBOSC)

$(LIBOSC): depend $(OBJS)
	$(RM) $@
	$(CXX) $(SHARED_LDFLAGS) -o $@ $(OBJS) $(LIBLO_LIBS)

glue.o: depend glue.cpp RTOscPField.h
	$(CXX) $(CXXFLAGS) -c -o glue.o glue.cpp

depend:
	-$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $(SRCS)' > depend

-include depend

install: all
	$(INSTALL) $(LIBOSC) $(LIBDESTDIR)

uninstall:
	$(RM) $(LIBDESTDIR)/$(LIBOSC)

clean:
	$(RM) *.o $(LIBOSC)

cleanall: clean
	$(RM) depend

