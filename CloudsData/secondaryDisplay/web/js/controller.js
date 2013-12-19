var paper;
var debug = false;

Controller = {	
	dragging: false,
	modes: {PROJECT_EXAMPLE:0, GRAPH:1},	
	onDataLoaded: function(peopleData, graphData, projectData){			
		GraphView.init();
		ProjectExampleView.init();
		
		this.mode = Controller.modes.GRAPH,
		//load data into Model			
		$(peopleData).find('person').each(Controller.addPerson);
			
		$(projectData).find('project').each(Controller.addProject);
		$(graphData).find('#nodes').find('circle').each(Controller.addClip);		
		$(graphData).find('#edges').find('path').each(Controller.addConnection);
		GraphView.resetZoomPan();
		
		//when all of these are done, 
		//$(peopleData).destroy()
		//$(graphData).destroy()
		
		Model.currentClip = Model.clips[0];
		Model.previousClip = Model.currentClip;
		
		sockets.setup(Controller.onStoryChanged);
		
		$('#graph').click(Controller.onClick); //for testing w/o running story engine
		$('#graph').mousedown(Controller.onMouseDown);
		$('#graph').mousemove(Controller.onMouseMove);
		$('#graph').mouseup(Controller.onMouseUp);
		$(window).keypress(Controller.onKeyPress);

		Controller.goToRandomProjectExample();
		
	 	Controller.stats = Controller.initStats();
		
		if (window.addEventListener)
		        /** DOMMouseScroll is for mozilla. */
		        window.addEventListener('DOMMouseScroll', Controller.wheel, false);
		/** IE/Opera. */
		window.onmousewheel = document.onmousewheel = Controller.wheel;		
		
		if(debug){
			console.log("loaded " + Model.people.length + " people into Model");
			console.log("loaded " + Model.clips.length + " clips into Model");
			console.log("loaded " + Model.connections.length + " connections into Model");
		}
	},
	addProject: function(){
		var project = new Project($(this).attr('title'), 
								$(this).find('creator_name').text(),
								$(this).find('description').text(),
								$(this).find('video_file_name').text()
								);
		Model.addProject(project);
	},
	addPerson: function(){
		var person = new Person($(this).attr('id'), 
								$(this).find('first').text(),
								$(this).find('last').text(),
								$(this).find('title').text(),
								$(this).find('twitter').text(),
								$(this).find('location').text(),
								$(this).find('byline1').text(),
								$(this).find('byline2').text(),
								$(this).find('resume').text()
								);
		Model.addPerson(person);
	
	},
	addClip: function(){	
		var clip = new Clip($(this).attr('class'),
							parseFloat($(this).attr('cx')),
							parseFloat($(this).attr('cy')),
							$(this).attr('fill')
							);
		Model.addClip(clip);		
		GraphView.addClip(clip);
				
	},
	addConnection: function(){				
		var connElement = $(this);
		var connectionID = connElement.attr('class');
		
		var clipA = null;
		var clipB = null;
		for(var i = 0; i < Model.clips.length; i++){
			var clip = Model.clips[i];			
			if(connectionID.indexOf(clip.id) >= 0){
				if(clipA == null){
					clipA = clip;					
				}
				else if(clipB == null){
					clipB = clip;
				}
			}
		}
				
		if(clipA != null && clipB != null){		
			Model.addConnection(clipA, clipB, $(this).attr('d'));
			//console.log("connection id: "  + connectionID + ". "+ clipA.id + " / " + clipB.id);
			GraphView.setNeighbors(clipA, clipB);	
		}
		else{
			if(debug){
				console.log("couldn't find corresponding nodes for this edge: '" + connectionID + "'");
			}
			
		}
		
	},
	onStoryChanged: function(message){		
		if(message.clip){		
			console.log(message.clip);	
			if(message.clip.example){
				//play example right away
				Controller.setCurrentProjectExample(message.clip.example);
				//just update model, don't update view:
				Model.setClip(message.clip.id); 
				Model.setPerson(message.clip.name); 
				Model.setTopic(message.clip.topic);
				Model.setQuestion(message.clip.question);
				Controller.setMode(Controller.modes.PROJECT_EXAMPLE);

			}
			else{
				Controller.setMode(Controller.modes.GRAPH);
				Controller.setCurrentClip(message.clip.id);
				Controller.setCurrentTopic(message.clip.topic);
				console.log(message.clip.question);
				//TODO: add question.

			}
		}	
		
	},
	onMouseDown: function(){
		
		this.dragging = true;
		this.dragStartX = event.clientX;
		this.dragStartY = event.clientY;
	},
	onClick: function(){
		//Controller.goToRandomClip();
	},
	onMouseMove: function(){
		if(this.dragging){
			
		}
	},
	onMouseUp: function(){
		if(this.dragging){
			var deltaX = event.clientX - this.dragStartX;
			var deltaY = event.clientY - this.dragStartY;
			
			var panX = deltaX > 0? -1: 1;
			var panY = deltaY > 0? -1: 1;
			
	        GraphView.pan(panX, panY);
			this.dragging = false;
		}
	},
	onKeyPress: function(){
		if ( event.keyCode == 32 ) { //space bar
			Controller.goToRandomClip();
			Controller.goToRandomTopic();
			Controller.setMode(Controller.modes.GRAPH);
		}
		if ( event.keyCode == 112 ) { //P key.
			Controller.goToRandomProjectExample();
			Controller.setMode(Controller.modes.PROJECT_EXAMPLE);
		}
	},	
	setMode: function(mode){
		Controller.mode = mode;
		if(Controller.mode == Controller.modes.PROJECT_EXAMPLE){
			$('#clip_info').fadeOut();
			$('#graph').fadeOut();
			$('#project_info').fadeIn();
			$('#video').fadeIn();
		}
		else{
			$('#project_info').fadeOut();
			$('#video').fadeOut();
			$('#clip_info').fadeIn();
			$('#graph').fadeIn();
		}
		
	},
	setCurrentClip: function(id){
		Model.setClip(id);
		GraphView.updateCurrentClip();	
		PersonView.update();
	},
	setCurrentTopic: function(topic){
		Model.setTopic(topic);
		GraphView.updateCurrentTopic();
		//TopicView.update();
	},	
	setCurrentProjectExample: function(project_title){
		Model.setProjectExample(project_title);
		ProjectExampleView.update();
	},	
	goToRandomClip: function(){
		var randomClip;
		if(Model.currentClip == null) Model.currentClip = Model.clips[0];
		if(Model.currentClip.neighbors.length == 0){ //jump to random clip
			var pos = Math.floor(Math.random() * Model.clips.length);
			randomClip = Model.clips[pos];	
		}
		else{
			var pos = Math.floor(Math.random() * Model.currentClip.neighbors.length);
			randomClip = Model.currentClip.neighbors[pos];	
		}	 
		this.setCurrentClip(randomClip.id);
		
	},	
	goToRandomTopic: function(){
		var topics = new Array();
		topics.push("politics");
		topics.push("confusion");
		topics.push("surprise");
		topics.push("apocalypse");
		topics.push("challenges");
		topics.push("design by numbers");
		topics.push("digital divide");
		topics.push("recursion");
		topics.push("improvisation");
		topics.push("abstraction");
		
		var pos = Math.floor(Math.random() * topics.length);
		this.setCurrentTopic(topics[pos]);
	},
	goToRandomProjectExample: function(){
		var projects = new Array();
		projects.push("Drawn1");
		projects.push("Drawn2");
		
		var pos = Math.floor(Math.random() * projects.length);
		this.setCurrentProjectExample(projects[pos]);
	},
	test: function(){
		console.log("loaded!");
	},
	init : function(){
		//loader.loadXML("../../links/clouds_links_db.xml"		
		$.when( loader.loadXML("xml/people.xml"), loader.loadXML("xml/2013_7_25_Clouds_conversation.svg") ,loader.loadXML("xml/projects.xml")).done(Controller.onDataLoaded);
			
	},
	wheel: function(event) {
	    var delta = 0;
		if (!event) /* For IE. */
		                event = window.event;
	    if (event.wheelDelta) { /* IE/Opera. */
	        delta = event.wheelDelta / 120;
	    } else if (event.detail) { /** Mozilla case. */
	        delta = -event.detail / 3;
	    }
	    if (delta) {					
			//var x = event.clientX;// - GraphView.paper.offsetLeft;
		    //var y = event.clientY;// - GraphView.paper.offsetTop;	
			
			var zoom = delta>0? 1:-1;
	        GraphView.zoom(zoom);
	    }
	    if (event.preventDefault) event.preventDefault();
	    event.returnValue = false;
	},
	initStats: function() {

        var stats = new Stats();
        stats.setMode(0); // 0: fps, 1: ms

        // Align top-left
        stats.domElement.style.position = 'absolute';
        stats.domElement.style.left = '0px';
        stats.domElement.style.top = '0px';

        $("#Stats-output").append(stats.domElement);

        return stats;
    }
	
	
	
	
}

$(document).ready(Controller.init);
