function Person(_id, _first, _last, _title, _twitter, _location, _byline1, _byline2, _resume, _clips) {	
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
	this.clips = new Array();
}

Person.prototype.addClip = function($clip) {
	this.clips.push($clip);
}

/*
Person.prototype.getClipByCLass = function(class) {
	for (var i=0; i<this.clips.length; i++) if (this.clips[i].class==class) return this.clips[i];
	return null;
}*/

