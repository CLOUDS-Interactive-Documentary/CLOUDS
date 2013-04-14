include ../../../makefile.conf

OBJS = RTcmixMIDI.o RTMidiPField.o glue.o
SRCS = RTcmixMIDI.cpp RTMidiPField.cpp glue.cpp

PTLIB = portmidi/porttime/libporttime.a

ifeq ($(ARCH), MACOSX)
	PMLIB = portmidi/pm_mac/libportmidi.a
	MIDISYSLIB = -framework CoreFoundation -framework CoreAudio -framework CoreMIDI
endif
ifeq ($(ARCH), LINUX)
	PMLIB = portmidi/pm_linux/libportmidi.a
#	MIDISYSLIB = -lasound
endif

CXXFLAGS += -I. -I../../rtcmix -Iportmidi/pm_common -Iportmidi/porttime

LIBMIDI = libmidiconn.so

all: $(LIBMIDI)

$(LIBMIDI): depend $(OBJS) $(PMLIB)
	$(RM) $@
	$(CXX) $(SHARED_LDFLAGS) -o $@ $(OBJS) $(PMLIB) $(PTLIB) $(MIDISYSLIB)

glue.o: depend glue.cpp RTMidiPField.h
	$(CXX) $(CXXFLAGS) -c -o glue.o glue.cpp

$(PMLIB):
	( cd portmidi; $(MAKE) $(MFLAGS) all; )

depend:
	-$(SHELL) -ec '$(CXX) -M $(CXXFLAGS) $(SRCS)' > depend

-include depend

install: all
	$(INSTALL) $(LIBMIDI) $(LIBDESTDIR)

uninstall:
	$(RM) $(LIBDESTDIR)/$(LIBMIDI)

clean:
	( cd portmidi; $(MAKE) $(MFLAGS) clean; )
	$(RM) *.o $(LIBMIDI)

cleanall: clean
	$(RM) depend

