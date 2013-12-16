function ClipView(_paper, _clip) {	
	this.paper = _paper;	
	this.clip = _clip;
	this.r = 5;
	this.x =  this.clip.originalX;
	this.y = this.clip.originalY;
	this.neighborViews = new Array();
	this.neighborPaths = new Array();
	this.selected = false;
	this.color = ColorPalette.getPaletteColor(this.clip.originalColor);//TODO: convert this.clip.originalFill to our palette. 
	
	this.circle = this.paper.circle(this.x, this.y, this.r)
						.attr("fill", this.getFill())
						.attr("stroke", "none");
	this.strokeWeight = 1;
	this.selected = false;
}

ClipView.prototype.addNeighbor = function(neighborView){
	this.neighborViews.push(neighborView);
	//update model
}

ClipView.prototype.getFill = function(){
	return this.color.toHexString();
}

ClipView.prototype.setFill = function(fill){
	this.circle.attr("fill", fill);
}


ClipView.prototype.select = function(){
	this.selected = true;
	this.circle.attr("fill",tinycolor.lighten(this.getFill(), 80).toHexString());
	this.currentGlow = this.circle.glow({color: "#FFFFFF", width: 10});
	
	
	for(var i = 0; i < this.neighborViews.length; i++){
		var neighbor = this.neighborViews[i];
		neighbor.circle.highlight(60);
		
		var pathToNeighbor = this.paper.path("M" + this.x + ", " + this.y + "L" + neighbor.x + "," + neighbor.y);
				pathToNeighbor.attr("z-index", -1);
				pathToNeighbor.attr("stroke", "#F1F1F1");
		pathToNeighbor.glowOnce(0, 0.5, 0.2);
		
		this.neighborPaths.push(pathToNeighbor);
		
	}
}

ClipView.prototype.deselect = function(){
	if(this.selected){
		if(this.currentGlow){
			this.currentGlow.remove();
		}
	
		
		for(var i = 0; i < this.neighborViews.length; i++){
			var neighbor = this.neighborViews[i];
			neighbor.circle.animate({ fill: neighbor.getFill() }, GraphView.timeUnit/2, 'linear');
		}
		
		for(var i = 0; i < this.neighborPaths.length; i++){			
			this.neighborPaths[i].fadeOut(function(){
				this.remove();
			});
		}
	}
	this.selected = false;
	
}
/*
ClipView.prototype.deleteNeighborPaths = function(){
	for(var i = 0; i < this.neighborPaths.length; i++){
		this.neighborPaths[i].remove();
	}
	this.neighborPaths = [];
	
}*/

