/* RTcmix - Copyright (C) 2011  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

// A simple Cocoa application that receives setup information from RTcmix
// over a socket, sends back mouse coordinates in range [0,1], and receives
// scaled data values for label display.  It's supposed to support the same
// functionality as the version for X.
//
// The reason for writing this separate program is that a command-line
// program (such as CMIX) can't put up a GUI window in OSX and still have
// events work correctly.
//
// -John Gibson, 8/11/11

#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
	return NSApplicationMain(argc, (const char **) argv);
}

