#pragma once

#include "ofMain.h"
#include "angleLengthLineMorpher.h"
#include "angleLengthLine.h"
#include "CloudsVisualSystem.h"

class linePoemApp {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        angleLengthLineMorpher   * ALM;
    
        vector < angleLengthLine > alines;
    
        void loadAllDrawings();
        void saveAllDrawings();
    
        ofPolyline line;
        ofTrueTypeFont font;

    
        ofFbo fbo;
    CloudsVisualSystem * SYS;
    
    
};
