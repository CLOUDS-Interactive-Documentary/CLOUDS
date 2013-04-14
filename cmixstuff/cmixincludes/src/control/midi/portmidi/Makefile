include ../../../../makefile.conf

# Define this for debugging only
PMFLAGS = -DPM_CHECK_ERRORS

PMOBJECTS = pm_common/pmutil.o pm_common/portmidi.o

PTOBJECTS = porttime/porttime.o
PTLIB = porttime/libporttime.a

ifeq ($(ARCH), MACOSX)
	VFLAGS = 
	PMLIB = pm_mac/libportmidi.a
	MIDISYSLIB = -framework CoreFoundation -framework CoreAudio \
		-framework CoreMIDI
	PMOBJECTS += pm_mac/pmmac.o pm_mac/pmmacosxcm.o
	PTOBJECTS += porttime/ptmacosx_cf.o 
#	PTOBJECTS += porttime/ptmacosx_mach.o 
endif
ifeq ($(ARCH), LINUX)
	VFLAGS = -DPMALSA
	PMLIB = pm_linux/libportmidi.a
	MIDISYSLIB = -lasound
	PMOBJECTS += pm_linux/pmlinux.o pm_linux/pmlinuxalsa.o
	PTOBJECTS += porttime/ptlinux.o 
endif

CFLAGS += $(VFLAGS) $(PMFLAGS) -Ipm_common -Iporttime


TESTPRG = pm_test/test pm_test/sysex pm_test/midithread pm_test/latency \
pm_test/midithru

all: $(PMLIB) $(PTLIB) $(TESTPRG)

$(PMLIB): Makefile $(PMOBJECTS)
	$(RM) $@
	ar qv $@ $(PMOBJECTS)
	$(RANLIB) $@

$(PTLIB): Makefile $(PTOBJECTS)
	$(RM) $@
	ar qv $@ $(PTOBJECTS)
	$(RANLIB) $@

pm_linux/pmlinuxalsa.o: Makefile pm_linux/pmlinuxalsa.c pm_linux/pmlinuxalsa.h
	$(CC) $(CFLAGS) -c pm_linux/pmlinuxalsa.c -o pm_linux/pmlinuxalsa.o

pm_test/test: Makefile pm_test/test.o $(PMLIB) $(PTLIB)
	$(CC) $(CFLAGS) pm_test/test.c -o pm_test/test $(PMLIB) $(PTLIB) \
		$(MIDISYSLIB)

pm_test/sysex: Makefile pm_test/sysex.o $(PMLIB) $(PTLIB)
	$(CC) $(CFLAGS) pm_test/sysex.c -o pm_test/sysex $(PMLIB) $(PTLIB) \
		$(MIDISYSLIB)

pm_test/midithread: Makefile pm_test/midithread.o $(PMLIB) $(PTLIB)
	$(CC) $(CFLAGS) pm_test/midithread.c -o pm_test/midithread \
		$(PMLIB) $(PTLIB) $(MIDISYSLIB)

pm_test/latency: Makefile $(PTLIB) pm_test/latency.o 
	$(CC) $(CFLAGS) pm_test/latency.c -o pm_test/latency $(PMLIB) $(PTLIB) \
		$(MIDISYSLIB) -lpthread -lm

pm_test/midithru: Makefile $(PTLIB) pm_test/midithru.o 
	$(CC) $(CFLAGS) pm_test/midithru.c -o pm_test/midithru $(PMLIB) $(PTLIB) \
		$(MIDISYSLIB) -lpthread -lm

porttime/ptlinux.o: Makefile porttime/ptlinux.c
	$(CC) $(CFLAGS) -c porttime/ptlinux.c -o porttime/ptlinux.o

clean:
	$(RM) *.o *~ core* */*.o */*~ */core*
	$(RM) $(PMLIB) $(PTLIB) pm_test/test pm_test/sysex pm_test/midithread
	$(RM) pm_test/latency pm_test/midithru

