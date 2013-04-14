README_LINUX.txt for PortMidi
Roger Dannenberg
8 June 2004

To make PortMidi and PortTime, go back up to the portmidi
directory and type make.

The Makefile will build all test programs and the portmidi
library. You may want to modify the Makefile to remove the
PM_CHECK_ERRORS definition. For experimental software,
especially programs running from the command line, we 
recommend using PM_CHECK_ERRORS -- it will terminate your
program and print a helpful message if any PortMidi 
function returns an error code.

If you do not compile with PM_CHECK_ERRORS, you should 
check for errors yourself.

This code has not been carefully tested; however, 
all test programs in pm_test seem to run properly.

CHANGELOG

08-Jun-2004 Roger B. Dannenberg
      Updated code to use new system abstraction.

12-Apr-2003 Roger B. Dannenberg
      Fixed pm_test/test.c to filter clocks and active messages.
      Integrated changes from Clemens Ladisch:
          cleaned up pmlinuxalsa.c
          record timestamp on sysex input
          deallocate some resources previously left open
