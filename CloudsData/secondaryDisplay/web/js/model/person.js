function Person(_id, _first, _last, _title, _twitter, _location, _byline1, _byline2, _resume, _nodes) {	
	this.id = _id;
	this.first = _first;
	this.last = _last;
	this.title = _title;
	this.twitter = _twitter;
	this.location = _location;
	this.byline1 = _byline1;
	this.byline2 = _byline2;
	this.resume = _resume;
	//this.view = new PersonView(this);
	this.nodes = new Array();
}

Person.prototype.addNode = function($node) {
	this.nodes.push($node);
}

/*
Person.prototype.getNodeByCLass = function(class) {
	for (var i=0; i<this.nodes.length; i++) if (this.nodes[i].class==class) return this.nodes[i];
	return null;
}*/

