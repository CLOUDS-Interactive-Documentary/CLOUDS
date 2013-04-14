# Print embeddable python library, as well as other libs it requires.
# Will prefer static linkage unless invoked with "shared" argument.
# -JGG, 8/4/04, rev. 7/21/05 for OS X Tiger

import sys, os.path, distutils.sysconfig

static_link = 1
nargs = len(sys.argv)
if nargs == 2 and sys.argv[1] == "shared":
   static_link = 0

major = sys.version_info[0]
minor = sys.version_info[1] * 0.1
pyversion = major + minor

# Note that this adds libraries we've certainly already linked to.
libs = distutils.sysconfig.get_config_var("LIBS")
libs += " " + distutils.sysconfig.get_config_var("SYSLIBS")

prefix = distutils.sysconfig.get_config_var("LIBPL")

if static_link:
   pythonlib = distutils.sysconfig.get_config_var("LIBRARY")
   if len(pythonlib) > 0:
      plib = prefix + '/' + pythonlib
      # Must see if file exists, because it doesn't in Jaguar!
      if os.path.exists(plib):
         print plib, libs
         sys.exit(0)
   # else try shared linkage...

linkshared = distutils.sysconfig.get_config_vars("LINKFORSHARED")[0]

if sys.platform.find("darwin") != -1:     # if it's OS X...
   if linkshared.find("Python") != -1:
      # If it's 10.3, <linkshared> includes "-framework Python", which is
      # what we want.  If it's 10.4, it does not include this, but rather
      # "Python.framework/Versions/2.3/Python", which is incorrect.  We
      # replace the latter with the former here.  It'd be nice to do this
      # in a less ad hoc way, but the combination of Apple's non-standard
      # way of doing things and distutils confusion prevents this.
      if linkshared.find("Python.framework/Versions") != -1:
         wrong = "Python.framework/Versions/" + str(pyversion) + "/Python"
         right = "-framework Python"
         linkshared = linkshared.replace(wrong, right, 1)
      print linkshared, libs
      sys.exit(0)
else:
   # On Linux, <linkshared> does not include the name of the shared library,
   # so we add it below.
   pythonlib = distutils.sysconfig.get_config_var("LDLIBRARY")
   if len(pythonlib) > 0:
      if pythonlib.endswith(".so"):       # sanity check
         plib = prefix + '/' + pythonlib
         if os.path.exists(plib):
            print linkshared, plib, libs
            sys.exit(0)

print >> sys.stderr, "***ERROR: Can't find a python library to embed."
sys.exit(1)

