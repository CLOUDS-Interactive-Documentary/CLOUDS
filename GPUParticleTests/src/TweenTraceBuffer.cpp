#include "TweenTraceBuffer.h"

//---------------------------------------------------
#pragma mark EasingFunction

//----------
TweenTraceBuffer::EasingFunction::EasingFunction(ofxEasing * function, ofxTween::ofxEasingType type) {
    this->function = function;
    this->type = type;
}

//----------
float TweenTraceBuffer::EasingFunction::tween(float value, float inputMin, float inputMax, float outputMin, float outputMax) const {
    return ofxTween::map(value, inputMin, inputMax, outputMin, outputMax, false, * this->function, this->type);
}

//---------------------------------------------------
#pragma mark KeyFrame

//----------
void TweenTraceBuffer::KeyFrame::draw() const {
	ofPushStyle();
	ofEnableSmoothing();
	ofSetColor(100, 255, 100);
	ofSetLineWidth(1.0f);

	ofMesh line, points;
	
	points.addVertex(ofVec3f(this->start.first, this->start.second, 0.0f));
	points.addVertex(ofVec3f(this->end.first, this->end.second, 0.0f));
	glPointSize(3.0f);
	points.setMode(OF_PRIMITIVE_POINTS);
	points.draw();

	
	for (float pct=0.0f; pct < 1.1f; pct += 0.1f) {
		const float time = (end.first - start.first) * pct + start.first;
		const float value = this->function->second.tween(time, start.first, end.first, start.second, end.second);
		line.addVertex(ofVec3f(time, value, 0.0f));
	}
	
	line.setMode(OF_PRIMITIVE_LINE_STRIP);
	line.draw();
	
	ofPopStyle();
}

//----------
bool TweenTraceBuffer::KeyFrame::operator<(const TweenTraceBuffer::KeyFrame & other) const {
	return this->start < other.start;
}

//---------------------------------------------------
#pragma mark InputBuffer

//----------
TweenTraceBuffer::InputBuffer::InputBuffer() {
	this->clear();
}

//----------
void TweenTraceBuffer::InputBuffer::add(time_t time, float value) {
	const pair<time_t, float> point(time, value);
	this->add(point);
}

//----------
void TweenTraceBuffer::InputBuffer::add(point point) {
	this->buffer.insert(point);
	if (point.first < this->start.first)
		this->start = point;
	if (point.first > this->end.first)
		this->end = point;
}

//----------
void TweenTraceBuffer::InputBuffer::clear() {
	this->start = pair<time_t, float>(numeric_limits<float>::max(), 0.0f);
	this->end = pair<time_t, float>(numeric_limits<float>::min(), 0.0f);
	this->buffer.clear();
}

//----------
void TweenTraceBuffer::InputBuffer::draw() const {
	ofPushStyle();
	ofEnableSmoothing();
	ofSetColor(255, 100, 100);
	ofSetLineWidth(1.0f);
	ofMesh mesh;
	
	map<time_t, float>::const_iterator it;
	for (it = this->buffer.begin(); it != this->buffer.end(); it++) {
		mesh.addVertex(ofVec3f(it->first, it->second, 0.0f));
	}
	
	glPointSize(3.0f);
	mesh.drawVertices();
	
	mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	mesh.draw();
	
	ofPopStyle();
}
	
//----------
size_t TweenTraceBuffer::InputBuffer::size() const {
	return this->buffer.size();
}
	
//----------
bool TweenTraceBuffer::InputBuffer::checkTrace(const EasingFunction & function, float threshold) const {
    
	if (this->buffer.size() < 3)
		return true;
	
    map<time_t, float>::const_iterator it = buffer.begin();
        
    it++; //start from second value
    
    for (int i=1; i<this->buffer.size(); i++, it++) {
        const float result = function.tween(it->first, this->start.first, this->end.first, this->start.second, this->end.second);
		
		if (result < it->second * (1.0f - threshold) || result > it->second * (1.0f + threshold)) {
			//outside acceptable range
			return false;
		}
    }
	
	return true;
}

//----------
TweenTraceBuffer::point TweenTraceBuffer::InputBuffer::getStart() const {
	return this->start;
}

//----------
TweenTraceBuffer::point TweenTraceBuffer::InputBuffer::getEnd() const {
	return this->end;
}

//----------
TweenTraceBuffer::point TweenTraceBuffer::InputBuffer::getEndMinusOne() const {
	if (this->buffer.size() < 3)
		return this->start;
	
	int targetIndex = this->buffer.size() - 2;
	map<time_t, float>::const_iterator it = this->buffer.begin();
	for (int i=0; i<targetIndex; i++)
		it++;
	
	return point(it->first, it->second);
}

//---------------------------------------------------
#pragma mark TweenTraceBuffer

//----------
TweenTraceBuffer::EasingDictionary * TweenTraceBuffer::easingFunctions = 0;

//----------
TweenTraceBuffer::TweenTraceBuffer(float threshold) : threshold(threshold) {
    if (this->easingFunctions == 0) {
        // intiialise easing functions
        this->easingFunctions = new EasingDictionary();
        
        this->addFunc("Linear", new ofxEasingLinear());
        this->addFunc("Exponential", new ofxEasingExpo());
        this->addFunc("Quadratic", new ofxEasingQuad());
        this->addFunc("Back", new ofxEasingBack());
        this->addFunc("Bounce", new ofxEasingBounce());
        this->addFunc("Circular", new ofxEasingCirc());
        this->addFunc("Quart", new ofxEasingQuart());
        this->addFunc("Quint", new ofxEasingQuint());
        this->addFunc("Elastic", new ofxEasingElastic());
        this->addFunc("Sine", new ofxEasingSine());
    }
    
    this->clear();
}

//----------
void TweenTraceBuffer::add(float time, float data) {
	this->inputBuffer.add(time, data);
	this->traceInputBuffer();
}

//----------
void TweenTraceBuffer::add(point point) {
	this->add(point.first, point.second);
}

//----------
void TweenTraceBuffer::clear() {
	this->clearInputBuffer();
	this->keyframes.clear();
}

//----------
void TweenTraceBuffer::flush() {
	current.start = inputBuffer.getStart();
	current.end = inputBuffer.getEndMinusOne();
	
	this->keyframes.insert(current);
	point endOfLast = this->inputBuffer.getEndMinusOne();
	this->clearInputBuffer();
	this->add(endOfLast);
}

//----------
void TweenTraceBuffer::draw() const {
	set<KeyFrame>::const_iterator it;
	for (it = this->keyframes.begin(); it != this->keyframes.end(); it++) {
		it->draw();
	}
	this->inputBuffer.draw();
}

//----------
float TweenTraceBuffer::getThreshold() const {
	return this->threshold;
}

//----------
void TweenTraceBuffer::setThreshold(float threshold) {
	this->threshold = threshold;
}


//----------
void TweenTraceBuffer::clearInputBuffer() {
	current.function = this->easingFunctions->find("Linear [in]");
	this->inputBuffer.clear();
}

//----------
bool TweenTraceBuffer::traceInputBuffer() {
    // This function checks whether we can approximate the entire input buffer
    // with one of our available easing functions.
    //
    // If we can't approximate well with an easing function, then we presume that
    // if we remove the last point, then we can approximate well with an easing function.
    //
    // This is always true, because we start off with 1 point, then 2 (can always be traced),
    // then 3 (maybe, and if so continue), etc...
    
    if (!this->searchFunc()) {
		// shave off last value
		// keyframe's function fits remaining points
		this->flush();
    }
}

//----------
void TweenTraceBuffer::addFunc(string name, ofxEasing * easingClass) {
    this->easingFunctions->insert(pair<string, EasingFunction>(name + " [in]",
                                                               EasingFunction(easingClass, ofxTween::easeIn)));
    this->easingFunctions->insert(pair<string, EasingFunction>(name + " [out]",
                                                               EasingFunction(easingClass, ofxTween::easeOut)));
    this->easingFunctions->insert(pair<string, EasingFunction>(name + " [in,out]",
                                                               EasingFunction(easingClass, ofxTween::easeInOut)));
}

//----------
bool TweenTraceBuffer::searchFunc() {
	//check each easing function for compatability with the dataset starting with current
	if (this->inputBuffer.checkTrace(current.function->second, this->threshold))
		return true;
	EasingDictionary::const_iterator it;
	
	for (it = easingFunctions->begin(); it != easingFunctions->end(); it++) {
		if(this->inputBuffer.checkTrace(it->second, this->threshold)) {
			this->current.function = it;
			return true;
		}
	}
	
	return false;
}







