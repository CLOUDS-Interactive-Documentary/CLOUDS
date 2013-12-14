function ConnectionView(_paper, _clipViewA, _clipViewB, _d, _stroke) {
	this.paper = _paper;
	this.clipViewA = _clipViewA;
	this.clipViewB = _clipViewB;
	
	//update model
	this.connection = new Connection(this.clipViewA.clip, this.clipViewB.clip);
	Model.addConnection(this.connection);
	
	this.clipViewA.addNeighbor(this.clipViewB);
	this.clipViewB.addNeighbor(this.clipViewA);
	
	this.d =  _d;
	//this.path = this.paper.path(this._d); //don't draw all connections.
	//.attr("stroke", _connection.clipA.getFill()); //connection stroke color determined by clips it connects
}
