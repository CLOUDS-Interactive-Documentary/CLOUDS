function NodeView(_parent, _node) {	
	this.scene = _parent;
	this.node = _node;

	this.r = GraphView.radius;
	//from (0, 1) range to (-1, 1); then scale.
	this.x =  (this.node.originalX * 2 - 1) * GraphView.scale;
	this.y = (this.node.originalY  * 2 - 1) * GraphView.scale;
	this.z = Math.random() * 50;

	// this.z = 20;
	this.neighborViews = new Array();
	//this.neighborPaths = new Array();
	this.neighborPaths = new THREE.Object3D();
	this.selected = false;

	this.color = ColorPalette.getPaletteColor(this.node.originalColor);		
	this.selectedColor = new THREE.Color("red");
	this.highlightedColor = new THREE.Color("white");
	this.z = this.color.getHSL().l * 50;
	

	var mapB = THREE.ImageUtils.loadTexture( "textures/sprite0.png" );
	mapB.needsUpdate = true;
	var materialB = new THREE.SpriteMaterial( { map: mapB, color: this.color, scaleByViewport:true, useScreenCoordinates:false } );
	this.material = materialB.clone();
	this.particle = new THREE.Sprite( this.material );	
	this.particle.position.set( this.x, this.y, this.z );
	this.particle.scale.set( this.r, this.r, 1);
	this.scene.add( this.particle );

}

NodeView.prototype.addNeighbor = function(neighborView){
	this.neighborViews.push(neighborView);
	//update model
}

NodeView.prototype.select = function(){

	this.selected = true;
	//this.particle.position.z += 3;
	this.particle.material.color = this.selectedColor; 
	//this.particle.scale = this.r * 10;

	// //TODO: GLOW


	//if I don't create the tween's callback in its own scope like this, the path variable is always the last one in the array (ah javascript!!!)
	var createFunction = function( current_i, path ) {
	    return function() {
	        	path.material.color.setHSL(0, 0, this.l);
	    	};
	};

	for(var i = 0; i < this.neighborViews.length; i++){
		var neighbor = this.neighborViews[i];
		neighbor.particle.material.color = this.highlightedColor;

		var geometry = new THREE.Geometry();
		var from = new THREE.Vector3(this.particle.position.x, this.particle.position.y, this.particle.position.z - 1 ); //so that path end is always behind circle
		var to = new THREE.Vector3(neighbor.particle.position.x, neighbor.particle.position.y, neighbor.particle.position.z - 1);  
		geometry.vertices.push(from);
		geometry.vertices.push(to);
		var pathToNeighbor = new THREE.Line( geometry, new THREE.LineBasicMaterial( ) );
		pathToNeighbor.material.color.setStyle("black");
		this.neighborPaths.add(pathToNeighbor);
		this.scene.add(this.neighborPaths);
		
		var l = pathToNeighbor.material.color.getHSL();
		var target = {h:0, s:0, l:1};
		tween = new TWEEN.Tween(l).to(target, 1000)
			.easing(TWEEN.Easing.Quadratic.Out)
			// In each iteration, call "createFunction", passing the current value of "path". 
     		// A function is returned that references the "path" value that was passed in. (otherwise, all tweens will reference the same (last) path. AH, JAVASCRIPT!!)
			 .onUpdate(createFunction(i, pathToNeighbor))
			.onComplete(function(){	
			
				})
			.start();
		
	}
	

}

NodeView.prototype.deselect = function(){
	if(this.selected){
		this.particle.material.color = this.color;
		// if(this.currentGlow){
		// 	this.currentGlow.remove();
		// }
		
		for(var i = this.neighborPaths.children.length; i >= 0; i--){
			var neighbor = this.neighborPaths.children[i];
			this.neighborPaths.remove(neighbor);
			// this.neighborPaths[i].fadeOut(function(){
			// 	this.remove();
			// });
		}
		this.selected = false;
	}
	
}


	
	

	

