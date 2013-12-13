//TODO: 
//change clip animation: first, just pan. when got to new node, zoom to bounding box. otherwise makes me dizzy. and also makes it clearer how moving to new node opens up new possibilities. 
//also, when calculating bounding box, do not fix aspect ratio, but do center box on screen.
//have a min and max width (for zooming in and out). enforce it. especially constrain the zooming in. add interface sliders to control min/max.
//add lifetime to visited paths
//make animation time-based, to be able to delete neighbor paths after they've faded out completely.
//implement topic change logic: set topicChanged flag. when clip changes, only fade out current / fade in new if it changed. (otherwise have it move with the panning?)
//mockup act change (triggering with button or key maybe): slight background color change
//add fps control (to monitor)
GraphView = {	
	clipViews: new Array(),//should be dictionary by ID for performance
	prevClipView: null,
	currClipView: null,
	traversedPaths: new Array(),
	history_length: 7,
	topicChanged: false,
	radius: 3, 
	stroke: 1,
	time_unit: 1000,	
	animating: false,
	min_bb_width: 500,
	max_bb_width: 2000,
	panDelta: 250,
	zoomDelta: 0.1,
	topicText: null,
	topic_x: 177,
	topic_y: 108,
	font_size: 100,
	padding: 400,
	bbDebug: null,
	init:function() {		
		this.width = $("#graph").width();
		this.height = $("#graph").height();
		this.paper = Raphael( 'graph', this.width, this.height );	
		
		this.currClipView = this.clipViews[0];	
		ColorPalette.init();
		
		var gui = new dat.GUI({
		  load: JSON,
		  preset: 'Default'
		});
		gui.remember(GraphView); 
		var graphLooks = gui.addFolder('Graph Looks');
		//graph looks
		graphLooks.add(GraphView, 'radius', 3, 20).onChange(GraphView.onGUIchanged);
		graphLooks.add(GraphView, 'stroke', 1, 5).onChange(GraphView.onGUIchanged);
		graphLooks.add(GraphView, 'history_length', 1, 20).onChange(GraphView.onGUIchanged);		
		graphLooks.open();
		
		var topic = gui.addFolder('Topic Text');
		topic.add(GraphView, 'font_size', 80, 100).onChange(GraphView.onGUIchanged);
		topic.add(GraphView, 'topic_x', 50, 400).onChange(GraphView.onGUIchanged);
		topic.add(GraphView, 'topic_y', 50, 400).onChange(GraphView.onGUIchanged);
		topic.open();
		
		var anim = gui.addFolder('Animation')
		anim.add(GraphView, 'time_unit', 600, 1500).onChange(GraphView.onGUIchanged);
		anim.add(GraphView, 'padding', 100, 400);
		anim.add(GraphView, 'min_bb_width', 100, 1000);
		anim.add(GraphView, 'max_bb_width', 1000, 2000);
		anim.open();
		
		
		
		
	},	
	addClip: function(clip){
		ColorPalette.addOriginalColor(clip.originalColor);
		this.clipViews.push(new ClipView(this.paper, clip));
		
	},
	setNeighbors: function(clipA, clipB){
		var clipViewA = this.getClipView(clipA);
		var clipViewB = this.getClipView(clipB);
		
		clipViewA.addNeighbor(clipViewB);
		clipViewB.addNeighbor(clipViewA);
	},
	updateCurrentClip: function(){		
		if(Model.previousClip !== null && Model.currentClip !== null){
			this.prevClipView = this.getClipView(Model.previousClip);
			this.currClipView = this.getClipView(Model.currentClip);	
			
			this.prevClipView.deselect();		
				
			this.drawPath(	this.prevClipView.x, 
							this.prevClipView.y, 
							this.currClipView.x, 
							this.currClipView.y, 
							this.prevClipView.getFill(), 
							this.currClipView.getFill(),
							GraphView.gotToCurrentClip);
			//this.zoomAndPan(clip.x, clip.y, 1);		//leap: no animation
			
			this.zoomAndPanTo(this.getBoundingBox(this.currClipView.neighborViews));
		}
	},
	drawPath: function(px, py, nx, ny, pcolor, ncolor, callback){
		//add paper path from prev clip to current clip (and animate this along with zoomPan)
		//unshift: add to the beginning of the array.
		this.traversedPaths.unshift(this.paper.path("M" + px + ", " + py + "L" + px + "," + py)
		.attr("stroke", pcolor)
		.attr("stroke-width", this.stroke)
		.animate( { 
			"path": "M" + px + ", " + py + "L" + nx + "," + ny, 
			"stroke": ncolor
			}, this.time_unit*2.2, callback));
			
		if(this.traversedPaths.length >= this.history_length){
			var pathToFadeOut = this.traversedPaths.pop();
			pathToFadeOut.fadeOut(function(){
				this.remove();
			});
		}
	},
	updateCurrentTopic: function(){
		this.topicChanged = true;
		if(this.topicText != null){
			this.topicText.fadeOut(function(){
				GraphView.topicText.remove();
			});
		}
		
		//this should be independent of clip update:
		//problem: apparently I get first clip change and then topic change.
		/*
		if(this.topicText != null){
			this.topicText.fadeOut();
		}
		this.topicText = this.paper.text(this.currClipView.x +220, this.currClipView.y + 80, Model.topic);
		this.topicText.attr("fill", "#FFFFFF")
								.attr("font-family", "Conv_MateriaPro_Light")
								.attr("font-size", "42px")
								.fadeIn(0, 1);
		*/
	},
	getClipView: function(clip){
		//doing this to avoid having the model depend on the view. might not be worth it (Clip could have an instance of clipView)
		//in anycase clipViews should be a dictionary, not an array.
		var clipView = null;
		for(var i = 0; i < this.clipViews.length; i++){
			if(this.clipViews[i].clip.id == clip.id){
				clipView = this.clipViews[i];
			}
		}
		return clipView;
	},
	gotToCurrentClip: function(){
		GraphView.currClipView.select();
		if(GraphView.topicChanged){
			GraphView.topicText = GraphView.paper.text(GraphView.currClipView.x + GraphView.topic_x, GraphView.currClipView.y + GraphView.topic_y, Model.topic);
			GraphView.topicText.attr("fill", "#FFFFFF")
									.attr("font-family", "Conv_MateriaPro_Light")
									.attr("font-size", "42px")
									.fadeIn(0, 1);
			GraphView.topicChanged = false;
		}
		
	},
	
	zoom: function(zoom){//1: zoom in. -1: zoom out.
		this.paper.zoom(1+zoom*this.zoomDelta);
	},
	pan: function(panX, panY){
		var currentScale = this.paper._viewBox[2]/this.defaultViewBox.w; //width
		this.paper.pan(panX*this.panDelta, panY*this.panDelta);
	},		
	getBoundingBox: function(clipViews){
		if(debug){
			if(this.bbDebug != null){
				this.bbDebug.remove();
			}
		}
		var minX, minY, maxX, maxY;
	
		if(this.currClipView == null){
			minX = Number.MAX_VALUE;
			minY = Number.MAX_VALUE;
			maxX  = Number.MIN_VALUE;
			maxY = Number.MIN_VALUE;
		}
		else{
			//take currClipView pos into account as well.
			minX = this.currClipView.x;
			minY = this.currClipView.y;
			maxX  = this.currClipView.x;
			maxY = this.currClipView.y;
		}
		
		for (var i=0; i< clipViews.length; i++){
			var clip = clipViews[i];
			if(clip.x < minX) minX = clip.x;
			if(clip.y < minY) minY = clip.y;
			if(clip.x > maxX) maxX = clip.x;
			if(clip.y > maxY) maxY = clip.y;
		}
		var bbWidth = maxX - minX; 
		var bbHeight = maxY - minY;
		//TODO: use min_bb_width & max_bb_width
		
		//TODO: should center the box 
		
		
		//if wanted to preserve aspect ratio: 
		/*
		if(bbWidth > 0 && bbHeight > 0 ){
			//preserve aspect ratio:
			if(bbWidth > bbHeight){ 
				bbHeight = bbWidth * this.height/this.width;
			}
			else{
				bbWidth = bbHeight * this.width/this.height;
			}
		}
		else{
			//by default, just use the size of the div (which is kind of random)
			bbWidth = this.width;
			bbHeight = this.height;
			
		}*/
		var bb = {x: minX, y:minY, w:bbWidth, h:bbHeight};
		
		if(debug){ //draw bounding box
			this.bbDebug = this.paper.rect(bb.x, bb.y, bb.w, bb.h)
			.attr("opacity", "0.4")
			.attr("fill", "none")
			.attr("stroke", "#FF0000");
		}
		return bb;
	},	
	zoomAndPanTo: function(bb){	
		//stop current transform animation
		if(this.animating){
			clearInterval( Raphael.fn.intervalID ); //stop previous viewbox animation
		}
		this.animating = true;
		//this.paper.animateViewBox(bb.x - padding, bb.y - padding, bb.w + this.padding*2, bb.h + this.padding*2, this.time_unit, '<>', function()
		
		
		var targetBox = this.centerBB(bb);
		this.paper.animateViewBox(targetBox.x, targetBox.y, targetBox.w, targetBox.h, this.time_unit, '<>', function()
		//this.paper.animateViewBox(bb.x - this.padding.x, bb.y - this.padding.y, bb.w + this.padding.x*2, bb.h + this.padding.y*2, this.time_unit, '<>', function()
		    {
				this.animating = false;
		        
		    } );
				
	},
	resetZoomPan: function(){
		this.defaultViewBox = this.centerBB(this.getBoundingBox(this.clipViews));
		this.paper.setViewBox(this.defaultViewBox.x, this.defaultViewBox.y, this.defaultViewBox.w, this.defaultViewBox.h);
	},
	centerBB: function(bb){
		var targetX, targetY, targetWidth, targetHeight;
		//center bbx
		if(bb.w >= bb.h){ 
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
	onGUIchanged: function(){
		//TODO: have separate events for separate controls, or ask what changed. otherwise, redrawing all circles when changing path witdth.
		for(var i = 0; i < GraphView.clipViews.length; i++){
			GraphView.clipViews[i].circle.attr("r", GraphView.radius); //TODO: add setRadius function (don't access all this directly)			
		}
		console.log(GraphView.font_size + "px");
		GraphView.topicText.attr('font-size', GraphView.font_size + "px");
		GraphView.topicText.attr('x', GraphView.currClipView.x + GraphView.topic_x);
		GraphView.topicText.attr('y', GraphView.currClipView.y + GraphView.topic_y);
	}
	
	
}
