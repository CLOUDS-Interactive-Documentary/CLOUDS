function Node(_class, _x, _y, _fill) {
	this.id = _class;
	this.originalX = _x;
	this.originalY = _y;
	this.originalColor = _fill; //original color. just using it as input data. 
	this.visited = false;
	this.neighbors = new Array();
}

Node.prototype.markAsVisited = function() {
	this.visited = true;
}

Node.prototype.addNeighbor = function(neighborNode){
	this.neighbors.push(neighborNode);
}





