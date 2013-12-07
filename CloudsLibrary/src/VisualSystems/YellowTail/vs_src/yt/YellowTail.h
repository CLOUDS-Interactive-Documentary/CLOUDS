#pragma once

#include "ofMain.h"
#include <deque>
#include <set>

#include "Gesture.h"
#include "YtPolygon.h"
#include "YTObserver.h"

#define YT_PROFILE_ON FALSE

#define MAX_GESTURES    20
#define MAX_TOUCHES             11

namespace YellowTail {
    
    class App : public ofBaseApp {
    public:
        void    touchDown(ofTouchEventArgs &touch);
        void    touchMoved(ofTouchEventArgs &touch);
        void    touchUp(ofTouchEventArgs &touch);
        void    touchDoubleTap(ofTouchEventArgs &touch);
        
        GestureStyle     getNewGestureStyle() { return mNewGestureStyle; }
        void    setNewGestureStyle(GestureStyle newGestureStyle) { mNewGestureStyle = newGestureStyle; }
        
        void    setObserver(YTObserver *observer) { mObserver = observer; }
        
        void    setup();
        void    update();
        void    draw();
        
        int     gestureCount() { return gestures.size(); }
        void	clearGestures();
        
    private:	       
        void	updateGeometry();
        void    fadeOutDrawToBeginIfNecessary();
        
        int		softMaxGestures;
        int		softMaxTouches;
        
        set<Gesture *> potentialGestures;
        deque<Gesture *> gestures;
        vector<Gesture *> fadingGestures;
        map<int, Gesture *> gestureForTouch;
        
        int		minimumTravelForNewPoint;
        
        int     previousActiveGestureCount;
        bool    neverTouchedBefore;
        
        GestureStyle mNewGestureStyle;
        
        YTObserver  *mObserver;
    };    
    
}