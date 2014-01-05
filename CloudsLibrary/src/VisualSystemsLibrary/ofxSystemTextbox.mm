//
//  ofxSystemTextbox.cpp
//  Voro
//
//  Created by James George on 1/5/14.
//
//

#include "ofxSystemTextbox.h"
#ifdef TARGET_OSX
// ofSystemUtils.cpp is configured to build as
// objective-c++ so as able to use NSAutoreleasePool.
// This is done with this compiler flag
//		-x objective-c++
// http://www.yakyak.org/viewtopic.php?p=1475838&sid=1e9dcb5c9fd652a6695ac00c5e957822#p1475838

#include <Carbon/Carbon.h>
#include <sys/param.h> // for MAXPATHLEN
#include <Cocoa/Cocoa.h>  // for NSAutoreleasePool
#endif

// create alert dialog
string ofxSystemTextBoxDialog(string question, string text){
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"OK"];
	[alert addButtonWithTitle:@"Cancel"];
	[alert setMessageText:[NSString stringWithCString:question.c_str()
											 encoding:NSUTF8StringEncoding]];
	// create text field
	NSTextField* label = [[NSTextField alloc] initWithFrame:NSRectFromCGRect(CGRectMake(0,0,300,40))];
	[label setStringValue:[NSString stringWithCString:text.c_str()
											 encoding:NSUTF8StringEncoding]];
	// add text field to alert dialog
	[alert setAccessoryView:label];
	NSInteger returnCode = [alert runModal];
	// if OK was clicked, assign value to text
	if ( returnCode == NSAlertFirstButtonReturn ){
		return [[label stringValue] UTF8String];
	}
	return "";
	
}
