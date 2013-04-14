# Makefile for $(TOP)/src

include ../makefile.conf

DIRS = sndlib audio conf parser control rtcmix

.PHONY: all progs dsos standalone install dso_install standalone_install \
uninstall dso_uninstall standalone_uninstall clean cleanall

all: $(DIRS)
	@for DIR in $(DIRS); \
	do \
	  ( cd $$DIR; $(MAKE) $(MFLAGS) all; ) \
	done

dsos: all

standalone: all

install:
	@for DIR in $(DIRS); \
	do \
	  ( cd $$DIR; echo "making install in $$DIR..."; \
	  $(MAKE) $(MFLAGS) install; ); \
	done

dso_install: all

standalone_install: standalone

uninstall:
	@for DIR in $(DIRS); \
	do \
	  ( cd $$DIR; echo "making uninstall in $$DIR..."; \
		$(MAKE) $(MFLAGS) uninstall ); \
	done

dso_uninstall:

standalone_uninstall: do_package

depend::
	@for DIR in $(DIRS); \
	do \
	  ( cd $$DIR; echo "making depend in $$DIR..."; \
	  $(RM) depend; \
	  $(MAKE) $(MFLAGS) depend ); \
	done

clean:
	@for DIR in $(DIRS); \
	do \
	  ( cd $$DIR; echo "making clean in $$DIR..."; \
		$(MAKE) $(MFLAGS) clean ); \
	done

cleanall::
	@for DIR in $(DIRS); \
	do \
	  ( cd $$DIR; echo "making cleanall in $$DIR..."; \
	  $(MAKE) $(MFLAGS) cleanall ); \
	done

