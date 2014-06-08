#include "ofxAudioUnit.h"
#include "ofxAudioUnitUtils.h"

#if !(TARGET_OS_IPHONE)
#include <CoreAudioKit/CoreAudioKit.h>
#include <AudioUnit/AUCocoaUIView.h>
#include <AudioUnit/AudioUnitCarbonView.h>

pascal OSStatus CarbonWindowEventHandler(EventHandlerCallRef nextHandlerRef, EventRef event, void * userData);

#pragma mark Objective-C

@interface ofxAudioUnitUIWindow : NSWindow
{
	NSView * _AUView;
	WindowRef _carbonWindow;
	AudioUnitCarbonView _carbonView;
	EventHandlerRef _carbonEventHandler;
}

+ (BOOL) audioUnitHasCocoaView:(AudioUnit)unit;
+ (BOOL) audioUnitHasCarbonView:(AudioUnit)unit;

- (void) initWithCocoaViewForUnit:(AudioUnit)unit;
- (void) initWithCarbonViewForUnit:(AudioUnit)unit;
- (void) initWithGenericViewForUnit:(AudioUnit)unit;
- (void) initWithAudioUnitCocoaView:(NSView *)audioUnitView;

- (id) initWithAudioUnit:(AudioUnit)unit forceGeneric:(BOOL)useGeneric;

- (void) audioUnitChangedViewSize:(NSNotification *)notification;

@property (readonly) WindowRef carbonWindow;

@end

@implementation ofxAudioUnitUIWindow

@synthesize carbonWindow = _carbonWindow;

// ----------------------------------------------------------
- (void) dealloc
// ----------------------------------------------------------
{
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:NSViewFrameDidChangeNotification
												  object:_AUView];
	
	if(_carbonView)         CloseComponent(_carbonView);
	if(_carbonWindow)       DisposeWindow(_carbonWindow);
	if(_carbonEventHandler) RemoveEventHandler(_carbonEventHandler);
	
	[super dealloc];
}

// ----------------------------------------------------------
- (id) initWithAudioUnit:(AudioUnit)unit forceGeneric:(BOOL)useGeneric
// ----------------------------------------------------------
{
	if(!useGeneric && [ofxAudioUnitUIWindow audioUnitHasCocoaView:unit])
	{
		[self initWithCocoaViewForUnit:unit];
	}
	else if(!useGeneric && [ofxAudioUnitUIWindow audioUnitHasCarbonView:unit])
	{
		[self initWithCarbonViewForUnit:unit];
	}
	else 
	{
		[self initWithGenericViewForUnit:unit];
	}
	
	return self;
}

// ----------------------------------------------------------
- (void) initWithCocoaViewForUnit:(AudioUnit)unit
// ----------------------------------------------------------
{
	UInt32  dataSize;
	Boolean isWriteable;
	AudioUnitCocoaViewInfo * cocoaViewInfo = NULL;
	UInt32  numberOfClasses;
	CFURLRef cocoaViewBundlePath = NULL;
	CFStringRef factoryClassName = NULL;
	NSView * AUView = nil;
	
	// getting the size of the AU View info
	OSStatus result = AudioUnitGetPropertyInfo(unit,
											   kAudioUnitProperty_CocoaUI,
											   kAudioUnitScope_Global,
											   0,
											   &dataSize,
											   &isWriteable);
	
	numberOfClasses = (dataSize - sizeof(CFURLRef)) / sizeof(CFStringRef);
	
	// getting the location / name of the necessary view factory bits
	if((result == noErr) && (numberOfClasses > 0))
	{
		cocoaViewInfo = (AudioUnitCocoaViewInfo *)malloc(dataSize);
		if(AudioUnitGetProperty(unit,
								kAudioUnitProperty_CocoaUI,
								kAudioUnitScope_Global,
								0,
								cocoaViewInfo,
								&dataSize) == noErr)
		{
			cocoaViewBundlePath = cocoaViewInfo->mCocoaAUViewBundleLocation;
			factoryClassName    = cocoaViewInfo->mCocoaAUViewClass[0];
		}
		else if(cocoaViewInfo != NULL)
		{
			free(cocoaViewInfo);
			cocoaViewInfo = NULL;
		}
	}
	
	// if we have everything we need, create the custom Cocoa view
	if(cocoaViewBundlePath && factoryClassName)
	{
		NSBundle * viewBundle = [NSBundle bundleWithURL:(NSURL *)cocoaViewBundlePath];
		if(viewBundle)
		{
			Class factoryClass = [viewBundle classNamed:(NSString *)factoryClassName];
			id<AUCocoaUIBase> factoryInstance = [[[factoryClass alloc] init] autorelease];
			AUView = [factoryInstance uiViewForAudioUnit:unit 
												withSize:NSMakeSize(0, 0)];
			// cleanup
			CFRelease(cocoaViewBundlePath);
			if(cocoaViewInfo)
			{
				for(int i = 0; i < numberOfClasses; i++)
					CFRelease(cocoaViewInfo->mCocoaAUViewClass[i]);
				free(cocoaViewInfo);
			}
		}
	}
	
	[self initWithAudioUnitCocoaView:AUView];
}

// ----------------------------------------------------------
- (void) initWithGenericViewForUnit:(AudioUnit)unit
// ----------------------------------------------------------
{
	AUGenericView * AUView = [[[AUGenericView alloc] initWithAudioUnit:unit] autorelease];
	[AUView setShowsExpertParameters:YES];
	[self initWithAudioUnitCocoaView:AUView];
}

// ----------------------------------------------------------
- (void) initWithAudioUnitCocoaView:(NSView *)audioUnitView
// ----------------------------------------------------------
{
	_AUView = audioUnitView;
	NSRect contentRect = NSMakeRect(0, 0, audioUnitView.frame.size.width, audioUnitView.frame.size.height);
	self = [super initWithContentRect:contentRect
							styleMask:(NSTitledWindowMask |
									   NSClosableWindowMask |
									   NSMiniaturizableWindowMask)
							  backing:NSBackingStoreBuffered
								defer:NO];
	if(self)
	{
		self.level = NSNormalWindowLevel;
		self.contentView = audioUnitView;
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(audioUnitChangedViewSize:)
													 name:NSViewFrameDidChangeNotification
												   object:_AUView];
	}
}

// ----------------------------------------------------------
- (void) initWithCarbonViewForUnit:(AudioUnit)unit
// ----------------------------------------------------------
{
	// This technique embeds a carbon window inside of a cocoa
	// window wrapper, as described by Technical Note TN2213
	// ("Audio Units: Embedding a Carbon View in a Cocoa Window")
	
	// Getting carbon view description
	UInt32 dataSize;
	Boolean isWriteable;
	
	OFXAU_RETURN(AudioUnitGetPropertyInfo(unit,
										  kAudioUnitProperty_GetUIComponentList,
										  kAudioUnitScope_Global,
										  0,
										  &dataSize,
										  &isWriteable),
				 "getting size of carbon view info");
	
	ComponentDescription * desc = (ComponentDescription *)malloc(dataSize);
	
	OFXAU_RETURN(AudioUnitGetProperty(unit,
									  kAudioUnitProperty_GetUIComponentList,
									  kAudioUnitScope_Global,
									  0,
									  desc,
									  &dataSize),
				 "getting carbon view info");
	
	ComponentDescription d = desc[0];
	
	// Creating carbon view component
	Component comp = FindNextComponent(NULL, &d);
	OFXAU_RETURN(OpenAComponent(comp, &_carbonView), "opening carbon view component");
	
	// Creating a carbon window for the view
	Rect carbonWindowBounds = {100,100,300,300};
	OFXAU_RETURN(CreateNewWindow(kPlainWindowClass,
								 (kWindowStandardHandlerAttribute |
								  kWindowCompositingAttribute),
								 &carbonWindowBounds,
								 &_carbonWindow),
				 "creating carbon window");
	
	// Creating carbon controls
	ControlRef rootControl, viewPane;
	OFXAU_RETURN(GetRootControl(_carbonWindow, &rootControl),
				 "getting root control of carbon window");
	
	// Creating the view
	Float32Point size = {0,0};
	Float32Point location = {0,0};
	OFXAU_RETURN(AudioUnitCarbonViewCreate(_carbonView,
										   unit,
										   _carbonWindow,
										   rootControl,
										   &location,
										   &size,
										   &viewPane),
				 "creating carbon view for audio unit");
	
	// Putting everything in place
	GetControlBounds(viewPane, &carbonWindowBounds);
	size.x = carbonWindowBounds.right  - carbonWindowBounds.left;
	size.y = carbonWindowBounds.bottom - carbonWindowBounds.top;
	SizeWindow(_carbonWindow, (short) (size.x + 0.5), (short) (size.y + 0.5), true);
	ShowWindow(_carbonWindow);
	
	// Listening to window events
	EventTypeSpec windowEventTypes[] = {
		{kEventClassWindow, kEventWindowGetClickActivation},
		{kEventClassWindow, kEventWindowHandleDeactivate}
	};
	
	EventHandlerUPP ehUPP = NewEventHandlerUPP(CarbonWindowEventHandler);
	OFXAU_RETURN(InstallWindowEventHandler(_carbonWindow,
										   ehUPP,
										   sizeof(windowEventTypes) / sizeof(EventTypeSpec),
										   windowEventTypes,
										   self,
										   &_carbonEventHandler),
				 "setting up carbon window event handler");
	
	NSWindow * wrapperWindow = [[[NSWindow alloc] initWithWindowRef:_carbonWindow] autorelease];
	
	self = [super initWithContentRect:NSMakeRect(0, 0, size.x + 1, size.y + 1)
							styleMask:(NSClosableWindowMask | 
									   NSMiniaturizableWindowMask |
									   NSTitledWindowMask)
							  backing:NSBackingStoreBuffered
								defer:NO];
	
	if(self)
	{
		[wrapperWindow setFrameOrigin:self.frame.origin];
		[self addChildWindow:wrapperWindow ordered:NSWindowAbove];
		self.level = NSNormalWindowLevel;
	}
}

pascal OSStatus CarbonWindowEventHandler(EventHandlerCallRef nextHandlerRef,
										 EventRef event,
										 void * userData)
{
	ofxAudioUnitUIWindow * uiWindow = (ofxAudioUnitUIWindow *)userData;
	UInt32 eventKind = GetEventKind(event);
	switch (eventKind) 
	{
		case kEventWindowHandleDeactivate:
			ActivateWindow([uiWindow carbonWindow], true);
			return noErr;
			
		case kEventWindowGetClickActivation:
			[uiWindow makeKeyAndOrderFront:nil];
			ClickActivationResult howToHandleClick = kActivateAndHandleClick;
			SetEventParameter(event, 
							  kEventParamClickActivation,
							  typeClickActivationResult,
							  sizeof(ClickActivationResult),
							  &howToHandleClick);
			return noErr;
	}
	
	return eventNotHandledErr;
}

// ----------------------------------------------------------
+ (BOOL) audioUnitHasCocoaView:(AudioUnit)unit
// ----------------------------------------------------------
{
	UInt32 dataSize;
	UInt32 numberOfClasses;
	Boolean isWriteable;
	
	OSStatus result = AudioUnitGetPropertyInfo(unit,
											   kAudioUnitProperty_CocoaUI,
											   kAudioUnitScope_Global,
											   0,
											   &dataSize,
											   &isWriteable);
	
	numberOfClasses = (dataSize - sizeof(CFURLRef)) / sizeof(CFStringRef);
	
	return (result == noErr) && (numberOfClasses > 0);
}

// ----------------------------------------------------------
+ (BOOL) audioUnitHasCarbonView:(AudioUnit)unit
// ----------------------------------------------------------
{
	UInt32 dataSize;
	Boolean isWriteable;
	OSStatus s = AudioUnitGetPropertyInfo(unit,
										  kAudioUnitProperty_GetUIComponentList,
										  kAudioUnitScope_Global,
										  0,
										  &dataSize, 
										  &isWriteable);
	
	return (s == noErr) && (dataSize >= sizeof(ComponentDescription));
}

// ----------------------------------------------------------
- (void) audioUnitChangedViewSize:(NSNotification *)notification
// ----------------------------------------------------------
{
	[[NSNotificationCenter defaultCenter] removeObserver:self 
													name:NSViewFrameDidChangeNotification 
												  object:_AUView];
	
	NSRect newRect = self.frame;
	NSSize newSize = [self frameRectForContentRect:((NSView *)[notification object]).frame].size;
	newRect.origin.y -= newSize.height - newRect.size.height;
	newRect.size = newSize;
	[self setFrame:newRect display:YES];
	
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(audioUnitChangedViewSize:) 
												 name:NSViewFrameDidChangeNotification
											   object:_AUView];
}

@end

#pragma mark - C++

using namespace std;

// ----------------------------------------------------------
void ofxAudioUnit::showUI(const string &title, int x, int y, bool forceGeneric)
// ----------------------------------------------------------
{
	if(!_unit.get()) return;
	
	NSString * windowTitle = [NSString stringWithUTF8String:title.c_str()];
	
	if(!windowTitle) {
		windowTitle = @"Audio Unit UI";
	}
	
	dispatch_async(dispatch_get_main_queue(), ^{
		@autoreleasepool {
			ofxAudioUnitUIWindow * auWindow = [[ofxAudioUnitUIWindow alloc] initWithAudioUnit:*_unit.get()
																				 forceGeneric:forceGeneric];
			
			CGFloat flippedY = [[NSScreen mainScreen] visibleFrame].size.height - y - auWindow.frame.size.height;
			
			[auWindow setFrameOrigin:NSMakePoint(x, flippedY)];
			[auWindow setTitle:windowTitle];
			[auWindow makeKeyAndOrderFront:nil];
		}
	});
}

#endif //TARGET_OS_IPHONE
