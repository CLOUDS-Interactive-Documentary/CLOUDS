function Connection(_nodeA, _nodeB, _originalCurve) {
	this.nodeA = _nodeA; 
	this.nodeB = _nodeB;
	this.originalCurveD = _originalCurve; //TODO: draw our own curves. Model shouldn't handle graphics.
	
	this.nodeA.addNeighbor(this.nodeB);
	this.nodeB.addNeighbor(this.nodeA);
}
