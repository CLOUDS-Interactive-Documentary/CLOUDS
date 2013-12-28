function Shape(){
	
}

Shape.prototype.createShape = function(shape, extrudeSettings, color, x, y, z, rx, ry, rz, s, _scene){
	var points = shape.createPointsGeometry();
	var spacedPoints = shape.createSpacedPointsGeometry( 100 );

	// solid line

	var line = new THREE.Line( points, new THREE.LineBasicMaterial( { color: color, linewidth: 2 } ) );
	line.position.set( x, y, z );
	line.rotation.set( rx, ry, rz );
	line.scale.set( s, s, s );
	return line;

	// flat shape

	// var geometry = new THREE.ShapeGeometry(shape);

	// var mesh = THREE.SceneUtils.createMultiMaterialObject( geometry, [ new THREE.MeshLambertMaterial( { color: color } ), new THREE.MeshBasicMaterial( { color: 0x000000, wireframe: true, transparent: true } ) ] );
	// mesh.position.set( x, y, z );
	// mesh.rotation.set( rx, ry, rz );
	// mesh.scale.set( s, s, s );
	// return mesh;
}

Rectangle.prototype = new Shape();       
Rectangle.prototype.constructor = Rectangle;       // Otherwise instances of Rectangle would have a constructor of Shape 


function Rectangle(_scene, _x, _y, _z, _width, _height, color) {
	this.scene = _scene;
	this.x = _x;
	this.y = _y;
	this.z = _z;
	this.w = _width;
	this.h = _height;

	//this.w = 120, this.h = 40;

	var rectShape = new THREE.Shape();
	rectShape.moveTo( 0,0 );
	rectShape.lineTo( 0, this.h );
	rectShape.lineTo( this.w , this.h );
	rectShape.lineTo( this.w , 0 );
	rectShape.lineTo( 0, 0 );

	var extrudeSettings = { amount: 20 }; // bevelSegments: 2, steps: 2 , bevelSegments: 5, bevelSize: 8, bevelThickness:5

	// addShape( shape, color, x, y, z, rx, ry,rz, s );
	extrudeSettings.bevelEnabled = true;
	extrudeSettings.bevelSegments = 2;
	extrudeSettings.steps = 2;
	this.rect = Shape.prototype.createShape( rectShape, extrudeSettings, color, this.x, this.y, this.z, 0, 0, 0, 1, this.scene);
	this.scene.add( this.rect );
}	

//TODO: these should just be in Shape superclass:
Rectangle.prototype.destroy = function(){
	this.scene.remove(this.rect);
}

Circle.prototype.destroy = function(){
	this.scene.remove(this.circle);
}

Circle.prototype = new Shape();        
Circle.prototype.constructor = Circle;       

//TODO: make Shape superclass, of course... not sure how to do inheritance in javascript right now.
function Circle(_scene, _x, _y, _z, _radius, color) {
	this.scene = _scene;
	this.x = _x;
	this.y = _y;
	this.z = _z;
	this.r = _radius;

	
	var circleShape = new THREE.Shape();
	circleShape.moveTo( 0, this.r );
	circleShape.quadraticCurveTo( this.r, this.r, this.r, 0 );
	circleShape.quadraticCurveTo( this.r, -this.r, 0, -this.r );
	circleShape.quadraticCurveTo( -this.r, -this.r, -this.r, 0 );
	circleShape.quadraticCurveTo( -this.r, this.r, 0, this.r );



	var extrudeSettings = { amount: 20 }; // bevelSegments: 2, steps: 2 , bevelSegments: 5, bevelSize: 8, bevelThickness:5

	// addShape( shape, color, x, y, z, rx, ry,rz, s );
	extrudeSettings.bevelEnabled = true;
	extrudeSettings.bevelSegments = 2;
	extrudeSettings.steps = 2;

	this.circle = Shape.prototype.createShape( circleShape, extrudeSettings, color, this.x, this.y, this.z, 0, 0, 0, 1, this.scene);
	this.scene.add( this.circle );
}	





