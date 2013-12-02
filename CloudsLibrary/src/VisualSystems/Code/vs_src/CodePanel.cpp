
#include "CodePanel.h"
#include "CloudsGlobal.h"

CodePanel::CodePanel(){
	offset = 0;
	scanSpeed = .1;
}

void CodePanel::setup(string codeFile){
	
	layout.loadFont( getDataPath() + "font/Courier New Bold.ttf", 8);
	
	initialBuffer = ofBufferFromFile(codeFile);
	while(!initialBuffer.isLastLine()){
		lines.push_back(initialBuffer.getNextLine());
	}
	
	offset = ofRandom(lines.size() - 40);
	//cout << "setup buffer " << initialBuffer.getText();
}

void CodePanel::update(){
	offset += scanSpeed;
//	cout << "offset is " << offset << endl;
}

void CodePanel::draw(){
//	layout.setLineLength( drawRect.width );
//	layout.drawString( initialBuffer.getText().substr(ofGetElapsedTimef()*10, 50), drawRect.x, drawRect.y );
	
	float lineHeight = layout.getStringBoundingBox("Q",0,0).height;
	int line = 0;
	
	ofSetColor(0);
	ofRect(drawRect);
	ofSetColor(255);

	for(int l = 0; l < drawRect.height; l += lineHeight*1.5){
		int curline = (line + int(offset)) % lines.size();
		layout.drawString( lines[curline], drawRect.x, drawRect.y + l);
		line++;
	}
}
