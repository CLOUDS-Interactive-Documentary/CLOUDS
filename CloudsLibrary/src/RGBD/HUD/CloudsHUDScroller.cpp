//
//  CloudsHUDLayer.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDScroller.h"


CloudsHUDScroller::CloudsHUDScroller(){
    bIsScrollUpHover = false;
    bIsScrollDownHover = false;
    bIsScrollUpPressed = false;
    bIsScrollDownPressed = false;
    bIsHoldScrolling = false;
    scrollPressedTime = 0;
    scrollIncrement = 0;
    scrollPosition = 0;
    
    
    bScrollBarHover = false;
    bScrollBarPressed = false;
    scrollBarAnchor = 0;
}

void CloudsHUDScroller::update(){
    if(bIsScrollDownPressed || bIsScrollUpPressed){
        float timeSincePress = ofGetElapsedTimef() - scrollPressedTime;
        bool scrolled = ofGetMousePressed() && ( ( bIsHoldScrolling && timeSincePress > .5) || (!bIsHoldScrolling && timeSincePress > .75) );
        if(scrolled){
            float newScrollPosition = scrollPosition + scrollIncrement * (bIsScrollUpPressed ? -1 : 1);
            scrollPosition = ofClamp(newScrollPosition, 0, totalScrollHeight - scrollBounds.height);
            bIsHoldScrolling = true;
        }
    }
}

bool CloudsHUDScroller::mouseMoved(ofVec2f pos){
    bIsScrollUpHover = scrollUpBoundsScaled.inside(pos);
    bIsScrollDownHover = scrollDownBoundsScaled.inside(pos);
    return bIsScrollUpHover || bIsScrollDownHover;
}

bool CloudsHUDScroller::mousePressed(ofVec2f pos){
    scrollPressedTime = ofGetElapsedTimef();
    
    bIsScrollUpPressed   = scrollUpBoundsScaled.inside(pos);
    bIsScrollDownPressed = scrollDownBoundsScaled.inside(pos);
    return bIsScrollDownPressed || bIsScrollUpPressed;
}

void CloudsHUDScroller::mouseDragged(ofVec2f pos){
    
}

void CloudsHUDScroller::mouseReleased(ofVec2f pos){
    if(bIsScrollUpPressed && scrollUpBoundsScaled.inside(pos)){
        float newScrollPosition = scrollPosition - scrollIncrement;
        scrollPosition = ofClamp(newScrollPosition, 0, totalScrollHeight - scrollBounds.height);
    }
    if(bIsScrollDownPressed && scrollDownBoundsScaled.inside(pos)){
        float newScrollPosition = scrollPosition + scrollIncrement;
        scrollPosition = ofClamp(newScrollPosition, 0, totalScrollHeight - scrollBounds.height);
    }
    bIsHoldScrolling = false;
}

void CloudsHUDScroller::mouseScrolled(float scroll){
    if(scrollBoundsScaled.inside(ofGetMouseX(), ofGetMouseY())){
        float newScrollPosition = scrollPosition + scrollIncrement * scroll;
       scrollPosition = ofClamp(newScrollPosition, 0, totalScrollHeight - scrollBounds.height);
    }

}

