include ../../../makefile.conf

MODULE = rtcmix
MODULE_SRC = $(MODULE)module.cpp
FUNCLIST = funclist
FUNCEXCEPT = funcexcept
FIND_FLAGS = -follow

.PHONY: all $(MODULE) install clean cleanall

all: $(MODULE)

# This dependency prevents making unless the funcexcept file has
# changed.  Not really what we want, but it's annoying to have
# to remake this stuff every time you make at the top level...
$(FUNCLIST): make_funclist.pl $(FUNCEXCEPT)
	$(PERL) -w make_funclist.pl $(CMIXDIR) $(FIND_FLAGS)

$(MODULE_SRC): make_python_ext.pl $(MODULE_SRC).in $(FUNCLIST)
	$(PERL) -w make_python_ext.pl $(MODULE) $(MODULE_SRC).in > $(MODULE_SRC)

# Mac OS X 10.3 fails to build shared lib, so complain if that's requested.
# If it's not requested, then ignore complaints about shared lib build failure
# from setup.py build.  Not sure what is causing the failure.
$(MODULE): $(MODULE_SRC)
ifeq ($(ARCH), MACOSX)
ifeq ($(PYTHON_EXT_STATIC_LINK), FALSE)
	@echo "Currently cannot build shared lib rtcmix extension under Mac OS X."
	@exit 1
endif
	# Build both Intel architectures; build fails for ppc arch with OS X 10.6
	# and gcc 4.2
	env CFLAGS='-arch i386 -arch x86_64' \
		LDFLAGS='-arch i386 -arch x86_64' $(PYTHON) setup.py build
	@echo "Don't worry about shared lib build failure.  We're using static link."
else
	$(PYTHON) setup.py build
endif
ifeq ($(PYTHON_EXT_STATIC_LINK), TRUE)
	$(RM) $(MODULE)module.o
	ln -s "`find . -name $(MODULE)module.o`" .
endif

# NB: need root for this; installs in python site-packages dir
install: all
ifeq ($(PYTHON_EXT_STATIC_LINK), FALSE)
	$(PYTHON) setup.py install
endif

# NB: unfortunately, distutils doesn't support uninstall 
uninstall:
#ifeq ($(PYTHON_EXT_STATIC_LINK), FALSE)
#	$(PYTHON) setup.py uninstall
#endif

clean:
	$(RM) $(FUNCLIST) $(MODULE).so $(MODULE)module.o $(MODULE_SRC)
	$(RM) -r build

cleanall: clean

depend:

