GraphView = {	
	nodeViews: new Array(),	
	prevNodeView: null,
	currNodeView: null,	
	traversedPaths: new Array(),
	history_length: 7,

	scale: 800,
	radius: 2,

	time_unit: 1000,

	showBB: false,
	padding: 10,
	currentRect: null,
	centeredRect:null,
	min_dist: 160,
	max_dist: 300,

	topic_x: 10,
	topic_y: 10,
	textMesh: null,	
	font_size: 10,
	
	setup:function() {		
		this.width = window.innerWidth;
		this.height = window.innerHeight;
		
		//SCENE
		this.scene = new THREE.Scene();

		//CAMERA
		this.viewAngle = 70;
		this.aspectRatio = this.width/this.height;
		this.nearPlane = 0.1;
		this.farPlane = 10000;
		this.camera = new THREE.PerspectiveCamera(this.viewAngle, this.aspectRatio, this.nearPlane, this.farPlane);
		this.camera.position.x = 200;
		this.camera.position.y = 100;
		this.camera.position.z = 200;

		//RENDERER
		if ( Detector.webgl )
			this.renderer = new THREE.WebGLRenderer({antialias:true});
		else
			this.renderer = new THREE.CanvasRenderer(); 
		this.renderer.setSize( this.width, this.height );
		this.renderer.autoClear = false; // To allow render overlay on top of sprited sphere

		container = document.getElementById('graph');
		container.appendChild( this.renderer.domElement);

		ColorPalette.init();

	},	
	update: function(){
		TWEEN.update();
	},
	render: function(){
        GraphView.renderer.render(GraphView.scene, GraphView.camera);
	},

	addNode: function(node){
		this.nodeViews.push(new NodeView(this.scene, node));
	},
	setNeighbors: function(nodeA, nodeB){
		var nodeViewA = this.getNodeView(nodeA);
		var nodeViewB = this.getNodeView(nodeB);
		
		nodeViewA.addNeighbor(nodeViewB);
		nodeViewB.addNeighbor(nodeViewA);
	},
	getNodeView: function(node){
		//doing this to avoid having the model depend on the view. might not be worth it (Node could have an instance of nodeView)
		//in anycase nodeViews should be a dictionary, not an array.
		var nodeView = null;
		for(var i = 0; i < this.nodeViews.length; i++){
			if(this.nodeViews[i].node.id == node.id){
				nodeView = this.nodeViews[i];
			}
		}
		return nodeView;
	},
	updateCurrentNode: function(){	
		if(Model.previousNode !== null && Model.currentNode !== null){
				this.prevNodeView = this.getNodeView(Model.previousNode);
				this.currNodeView = this.getNodeView(Model.currentNode);	
				
				this.prevNodeView.deselect();	
					
				this.addPath(GraphView.gotToCurrentNode);
				
				//this.zoomAndPanTo(this.getBoundingBox(this.nodeViews)); //this should be done at init.
				// THIS IS THE ACTUAL ONE!!!
				this.zoomAndPanTo(this.getBoundingBox(this.currNodeView.neighborViews));
				
				
		
			}
	},
	gotToCurrentNode: function(){
		GraphView.currNodeView.select();
		if(Model.topicChanged){	
			if(GraphView.textMesh){
				GraphView.scene.remove(GraphView.textMesh);
			}
			// LIGHT
			var light = new THREE.PointLight(0xffffff);
		    light.position.set(0,0,1000);
		    GraphView.scene.add(light);

		    var material = new THREE.MeshPhongMaterial({
		        color: 0xdddddd
		    });

		    var textGeom = new THREE.TextGeometry( Model.topic, {
		        font: 'blender',
		        size: GraphView.font_size,
		        height: 0
		    });

		    GraphView.textMesh = new THREE.Mesh( textGeom, material );
		    GraphView.textMesh.dynamic = true;

		    textGeom.computeBoundingBox();
		    var textWidth = textGeom.boundingBox.max.x - textGeom.boundingBox.min.x;
		    
		     var x = GraphView.currNodeView.x + GraphView.topic_x;
		     var y = GraphView.currNodeView.y + GraphView.topic_y;
		     var z = GraphView.currNodeView.z;
		    // textMesh.position.set(x, y, z );
		    GraphView.textMesh.position.set(x, y, z );
		    GraphView.scene.add( GraphView.textMesh ); 
		    Model.topicChanged = false;
		}
		

	},
	updateCurrentTopic: function(){
		Model.topicChanged = true;
		if(this.topicText != null){
			// this.topicText.fadeOut(function(){
			// 	GraphView.topicText.remove();
			// });
		}
		
		//this should be independent of node update:
		//problem: apparently I get first node change and then topic change.
		/*
		if(this.topicText != null){
			this.topicText.fadeOut();
		}
		this.topicText = this.paper.text(this.currNodeView.x +220, this.currNodeView.y + 80, Model.topic);
		this.topicText.attr("fill", "#FFFFFF")
								.attr("font-family", "Conv_MateriaPro_Light")
								.attr("font-size", "42px")
								.fadeIn(0, 1);
		*/
	},

	getBoundingBox: function(nodeViews){

		if(debug){
			if(this.bbDebug != null){
				this.bbDebug.remove();
			}
		}
		var minX, minY, maxX, maxY;
	
		if(this.currNodeView == null){
			this.currNodeView = nodeViews[0];
		}
		
		minX = this.currNodeView.x;
		minY = this.currNodeView.y;
		maxX  = this.currNodeView.x;
		maxY = this.currNodeView.y;
		
		for (var i=0; i< nodeViews.length; i++){
			var node = nodeViews[i];

			var dx = node.x - minX;
			var dy = node.y - minY;

			var dist = Math.sqrt(dx*dx + dy*dy);
			if(dist < this.max_dist){ //ignore points that are too far
				if(node.x < minX) minX = node.x;
				if(node.y < minY) minY = node.y;
				if(node.x > maxX) maxX = node.x;
				if(node.y > maxY) maxY = node.y;
			}

			
		}
		var bbWidth = maxX - minX; 
		var bbHeight = maxY - minY;
		//TODO: use min_bb_width & max_bb_width

		//all neighbors too near / too far:
		if(bbWidth < this.min_dist) bbWidth = this.min_dist; 		
		if(bbHeight < this.min_dist) bbHeight = this.min_dist;
		
		var bb = {x: minX, y:minY, w:bbWidth, h:bbHeight};
		
		if(debug){ //draw bounding box
			this.bbDebug = this.paper.rect(bb.x, bb.y, bb.w, bb.h)
			.attr("opacity", "0.4")
			.attr("fill", "none")
			.attr("stroke", "#FF0000");
		}
		
		// console.log("minX: " + minX + ", minY: " + minY + ", maxX: " + maxX + "maxY: " + maxY );
		
		return bb;

	},
	centerBB: function(bb){
		var targetX, targetY, targetWidth, targetHeight;
		//center bbx
		if(bb.w > bb.h){ 
			targetWidth = bb.w + this.padding*2;
			targetHeight = targetWidth * this.height/this.width;
			targetX = bb.x - this.padding;
			targetY = bb.y - (targetHeight - bb.h)/2;
			
		}
		else{
			targetHeight = bb.h + this.padding*2;
			targetWidth = targetHeight * this.width/this.height;
			targetY = bb.y - this.padding;
			targetX = bb.x - (targetWidth - bb.w)/2;
			
		}
		return {x: targetX, y:targetY, w:targetWidth, h:targetHeight};
	},

	constrainBB: function(bb, targetBox){
		var targetX, targetY, targetWidth, targetHeight;

		//center bbx
		if(bb.w > bb.h){ 
			targetWidth = bb.w + this.padding*2;
			targetHeight = targetWidth *targetBox.h/targetBox.w;
			targetX = bb.x - this.padding;
			targetY = bb.y - (targetHeight - bb.h)/2;
			
		}
		else{
			targetHeight = bb.h + this.padding*2;//only padding below
			targetWidth = targetHeight * targetBox.w/targetBox.h;
			targetY = bb.y - this.padding;//only padding below
			targetX = bb.x - (targetWidth - bb.w)/2;
			
		}
		return {x: targetX, y:targetY, w:targetWidth, h:targetHeight};
	},	

	zoomAndPanTo: function(bb){	
		if(this.currentRect != null){
			this.currentRect.destroy();
			this.centeredRect.destroy();
		}

		// var cb = this.centerBB(bb);
		var upperThirds = {x:0, y:0, w:this.width, h:this.height};///3*2
		var cb = this.constrainBB(bb, upperThirds);
		
		if(this.showBB){
			//draw bounding boxes
			this.currentRect = new Rectangle(this.scene, bb.x, bb.y, 25, bb.w, bb.h, 0x00FF00);
			this.centeredRect = new Rectangle(this.scene, cb.x, cb.y, 25, cb.w, cb.h, 0xff0000);
		}

		var pos = this.camera.position.clone();

		// var cbCenterX = cb.x + cb.w/2;		
		// var cbCenterY = cb.y + cb.h/2;
		var cbCenterX = GraphView.currNodeView.x;
		var cbCenterY = GraphView.currNodeView.y;

		var targetZ = cb.h/2 / Math.tan(this.viewAngle/2);
		var target = new THREE.Vector3( cbCenterX, cbCenterY, targetZ );
		
		var tween = new TWEEN.Tween(pos).to(target, this.time_unit);
		tween.easing(TWEEN.Easing.Quadratic.InOut);
		tween.onUpdate(function(){
			GraphView.camera.position.x = pos.x;
			GraphView.camera.position.y = pos.y;
			GraphView.camera.position.z = pos.z;
		});
		tween.onComplete(function(){	
			
		});
		tween.start();
				
	},
	addPath: function(callback){
		
		var geom = new THREE.Geometry();
		geom.vertices.push(this.prevNodeView.particle.position.clone());
		geom.vertices.push(this.prevNodeView.particle.position.clone());		
		geom.dynamic = true;
		var material = new THREE.LineBasicMaterial();
		//material.color = this.currNodeView.color;
		material.color = new THREE.Color("red");
		var path = new THREE.Line(geom, material);
		this.scene.add(path);

		var position = path.geometry.vertices[0];
		position.z = position.z - 1; //path ends should be behind circle.
		var target = this.currNodeView.particle.position.clone();
		target.z = target.z - 1;
		
		
		
		var tween = new TWEEN.Tween(position).to(target, this.time_unit)
						.easing(TWEEN.Easing.Quadratic.InOut)
						.onUpdate(function(){
							 path.geometry.vertices.push(this.x, this.y, this.z);
							 //path.geometry.__dirtyVertices = true;							 
							 path.geometry.verticesNeedUpdate = true;
							//path.geometry.vertices[0].set(this.x, this.y, this.z);
							//console.log(this.x);
						})
						.onComplete(function(){
							callback();
						})
						.start();

		// var tween = new TWEEN.Tween(this.prevNodeView.color.clone().getHSL()).to(this.currNodeView.color.getHSL(), this.time_unit)
		// 				.easing(TWEEN.Easing.Quadratic.InOut)
		// 				.onUpdate(function(){
		// 					 path.material.color.setHSL(this.h, this.s, this.l);
		// 				})
		// 				.start();



		// unshift: add to the beginning of the array.
		this.traversedPaths.unshift(path);
		
			
		if(this.traversedPaths.length >= this.history_length){
			var pathToFadeOut = this.traversedPaths.pop();
			this.scene.remove(pathToFadeOut);
			// pathToFadeOut.fadeOut(function(){ //ANIMATE
			// 	// this.remove(); 
			// });
		}
		
	}
	
	
}
