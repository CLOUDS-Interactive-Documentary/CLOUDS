/* RTcmix - Copyright (C) 2011  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#import <Cocoa/Cocoa.h>

@interface MouseWindowAppDelegate : NSObject <NSApplicationDelegate> {
@private
	NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
