function Connection(_clipA, _clipB, _originalCurve) {
	this.clipA = _clipA; 
	this.clipB = _clipB;
	this.originalCurveD = _originalCurve; //TODO: draw our own curves. Model shouldn't handle graphics.
	
	this.clipA.addNeighbor(this.clipB);
	this.clipB.addNeighbor(this.clipA);
}
