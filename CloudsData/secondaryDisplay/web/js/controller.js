
var debug = false;

Controller = {	
	modes: {PROJECT_EXAMPLE:0, GRAPH:1},	
	setup: function(peopleData, graphData, projectsData){		
		this.mode = Controller.modes.GRAPH;

		Controller.loadModel(peopleData, graphData, projectsData);

		//Interface Controls
		Controller.stats = Controller.initStats();	

		//Events
		sockets.setup(Controller.onStoryChanged);	

		
		$('#graph').click(Controller.onClick); //for testing w/o running story engine
		$('#graph').mousedown(Controller.onMouseDown);
		$('#graph').mousemove(Controller.onMouseMove);
		$('#graph').mouseup(Controller.onMouseUp);
		$(window).keypress(Controller.onKeyPress);

		$('#video').bind('ended', function(){
 			Controller.setMode(Controller.modes.GRAPH);
	 	})

	 	// STATS
		stats = new Stats();
		stats.domElement.style.position = 'absolute';
		stats.domElement.style.bottom = '0px';
		stats.domElement.style.zIndex = 100;
		container = document.getElementById('graph');//TODO: maybe have a different div for the interface (not threejs)
		container.appendChild( stats.domElement );

		// GUI
	 	var gui = new dat.GUI({
		  load: JSON,
		  preset: 'Default'
		});
		gui.remember(Controller); 

		var graphLooks = gui.addFolder('Graph Looks');
		
		// graphLooks.add(GraphView, 'radius', 3, 20).onChange(GraphView.onGUIchanged);
		graphLooks.add(GraphView, 'history_length', 1, 20).onChange(GraphView.onGUIchanged);		
		graphLooks.open();
		
		var topic = gui.addFolder('Topic Text');
		topic.add(GraphView, 'font_size', 5, 100).onChange(GraphView.onGUIchanged);
		topic.add(GraphView, 'topic_x', 0, 200).onChange(GraphView.onGUIchanged);
		topic.add(GraphView, 'topic_y', 0, 200).onChange(GraphView.onGUIchanged);
		topic.open();
		
		var anim = gui.addFolder('Animation')
		anim.add(GraphView, 'time_unit', 600, 1500).onChange(GraphView.onGUIchanged);
		anim.add(GraphView, 'showBB');
		anim.add(GraphView, 'padding', 0, 100);
		anim.add(GraphView, 'min_dist', 50, 500);
		anim.add(GraphView, 'max_dist', 100, 1000);
		anim.open();
		

		// if (window.addEventListener)
		//         /** DOMMouseScroll is for mozilla. */
		//         window.addEventListener('DOMMouseScroll', Controller.wheel, false);
		// /** IE/Opera. */
		// window.onmousewheel = document.onmousewheel = Controller.wheel;	

		
		if(debug){
			console.log("loaded " + Model.people.length + " people into Model");
			console.log("loaded " + Model.clips.length + " clips into Model");
			console.log("loaded " + Model.connections.length + " connections into Model");
		}
	},
	loop: function(){
		requestAnimationFrame( Controller.loop );
		GraphView.update();	
		// Controller.controls.update();	
		stats.update();
		GraphView.render();	
	},
	setCurrentNode: function(id){
		Model.setNode(id);
		GraphView.updateCurrentNode();
		//Model.setPerson(message.node.name);
		//PersonView.update();
	},
	setCurrentTopic: function(topic){
		Model.setTopic(topic);
		GraphView.updateCurrentTopic();
		//$('#topic').html(Model.topic);
	},	
	setCurrentQuestion: function(question){
		Model.setQuestion(question);	
		$('#question').html(Model.question);
		//TopicView.update();
	},	
	setCurrentPerson: function(person){
		Model.setPerson(person);	
		var person = Model.currentPerson;
		if(person){
			$('#first').html(person.first);
			$('#last').html(person.last);
			$('#title').html(person.title);
			$('#location1').html(person.location);
			$('#byline').html(person.byline1);
			$('#description').html(person.byline2);

			if(debug){
				console.log("first: " + person.first 
						+ ", last: " + person.last 
						+ ", title: " + person.title 
						+ ", location: " + person.location 
						+ ", byline: " + person.byline1 
						+ ", description: " + person.description 
				);
			}
			
		}
		//TopicView.update();
	},	
	setCurrentProjectExample: function(project_title){
		Model.setProjectExample(project_title);
		var project = Model.projectExample;
		if(project){
			$('#projectex_title').html(project.title);
			$('#projectex_artist').html(project.creatorName);
			$('#projectex_desc').html(project.description);
			$('#video').attr("source", project.video_file_name);
			$('#video').attr("src", project.videoFileName);
			$('#video').load();
		}
		$('#video').get(0).play();
	},	
	onStoryChanged: function(message){
		if(debug){
			console.log("received message: " + message.clip);
		}

		if(message.clip){		
			
			if(message.clip.example){
				Controller.setMode(Controller.modes.PROJECT_EXAMPLE);
				//play example right away
				Controller.setCurrentProjectExample(message.clip.example);
				//just update model, don't update view:
				Model.setNode(message.clip.id); 
				Model.setPerson(message.clip.name); 
				Model.setTopic(message.clip.topic);
				Model.setQuestion(message.clip.question);
				Controller.setCurrentQuestion(message.clip.question);

			}
			else{
				Controller.setMode(Controller.modes.GRAPH);
				Controller.setCurrentNode(message.clip.id);
				Controller.setCurrentPerson(message.clip.name);
				Controller.setCurrentTopic(message.clip.topic);				
				Controller.setCurrentQuestion(message.clip.question);

			}
		}	
		

	},	
	onKeyPress: function(){
		var dummyMessage;
		if ( event.keyCode == 32 ) { //space bar
			// Controller.goToRandomNode();
			// Controller.goToRandomTopic();
			
			dummyMessage = {
				clip: {
					duration: 39.998333,
					id: Controller.getRandomNode(),
					name: "Shantell",
					question: "How do you sketch with code FAKE?",
					topic: Controller.getRandomTopic()
				}

			}
			
		}
		else if ( event.keyCode == 112 ) { //P key.
			dummyMessage = {
				clip: {
					duration: 39.998333,
					id: Controller.getRandomNode(),
					example: Controller.getRandomProjectExample(),
					name: "Zach Lieberman",
					question: "A fake question",
					topic: Controller.getRandomTopic()
				}

			}
		}
		Controller.onStoryChanged(dummyMessage);
		
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
	
	getRandomNode: function(){
		var randomNode;
		if(Model.currentNode == null) Model.currentNode = Model.nodes[0];
		if(Model.currentNode.neighbors.length == 0){ //jump to random node
			var pos = Math.floor(Math.random() * Model.nodes.length);
			randomNode = Model.nodes[pos];	
		}
		else{
			var pos = Math.floor(Math.random() * Model.currentNode.neighbors.length);
			randomNode = Model.currentNode.neighbors[pos];	
		}	 
		return randomNode.id;
		
	},	
	getRandomTopic: function(){
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
		return topics[pos];
	},	
	getRandomProjectExample: function(){
		var projectTitles = new Array();
		projectTitles.push("Drawn");
		projectTitles.push("Flight Patterns");
		projectTitles.push("Messa Di Voce");
		projectTitles.push("Listening Post");
		projectTitles.push("Good Morning!");
		projectTitles.push("Cascade");
		projectTitles.push("Transparency Grenade");
		projectTitles.push("Cell Cycle");
		projectTitles.push("Communion");
		projectTitles.push("Aesthetics and Computation Group");
		
		var pos = Math.floor(Math.random() * projectTitles.length);
		return projectTitles[pos];
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
    },
    loadModel: function(peopleData, graphData, projectsData){
    	//Find min and max positions in SVG, so that we can normalize positions later
		var minX = Number.MAX_VALUE;
		var minY = Number.MAX_VALUE;
		var maxX = Number.MIN_VALUE;
		var maxY = Number.MIN_VALUE;

		$(graphData).find('#nodes').find('circle').each(function(){
			var x = parseFloat($(this).attr('cx'));
			var y = parseFloat($(this).attr('cy'));
			if(x < minX) minX = x;
			if(y < minY) minY = y;
			if(x > maxX) maxX = x;
			if(y > maxY) maxY = y;		
		});

		if(debug){
			console.log("minX:" + minX 
					+ " | minY:" + minY
					+ " | maxX:" + maxX
					+ " | maxY:" + maxY
					);
		}
		
		//PEOPLE
		$(peopleData).find('person').each(function(){
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
			if(debug){
				console.log("first: " + person.first 
							+ ", last: " + person.last 
							+ ", title: " + person.title 
							+ ", location: " + person.location 
							+ ", byline: " + person.byline1 
							+ ", description: " + person.description 
					);
		 	}	
			Model.addPerson(person);
		});

		$(projectsData).find('project').each(function(){
			var project = new Project($(this).attr('title'), 
								$(this).find('creator_name').text(),
								$(this).find('description').text(),
								$(this).find('video_file_name').text()
								);
			Model.addProject(project);

		});
		

		//NODES
		GraphView.setup();	
		// this.controls = new THREE.OrbitControls( GraphView.camera, GraphView.renderer.domElement );
		// this.controls.addEventListener( 'change', GraphView.render );
		 
		

		$(graphData).find('#nodes').find('circle').each(function(){
			var x = parseFloat($(this).attr('cx'));
			var y = parseFloat($(this).attr('cy'));
			var normalizedX = (x - minX) / (maxX - minX);
			//var normalizedY = (y - minY) / (maxY - minY);
			var normalizedY = (y - minY) / (maxX - minX);
			//GraphView.originalWidth = (maxX - minX);
			//GraphView.originalHeight = (maxY - minY);
			
			var node = new Node($(this).attr('class'),
							normalizedX,
							normalizedY,
							$(this).attr('fill')
							);
			Model.addNode(node);		
			GraphView.addNode(node);
		});
		//CONNECTIONS
		$(graphData).find('#edges').find('path').each(function(){
			var connElement = $(this);
			var connectionID = connElement.attr('class');
			
			var nodeA = null;
			var nodeB = null;
			for(var i = 0; i < Model.nodes.length; i++){
				var node = Model.nodes[i];			
				if(connectionID.indexOf(node.id) >= 0){
					if(nodeA == null){
						nodeA = node;					
					}
					else if(nodeB == null){
						nodeB = node;
					}
				}
			}
					
			if(nodeA != null && nodeB != null){		
				Model.addConnection(nodeA, nodeB, $(this).attr('d'));
				//console.log("connection id: "  + connectionID + ". "+ nodeA.id + " / " + nodeB.id);
				GraphView.setNeighbors(nodeA, nodeB);	
			}
			else{
				console.log("couldn't find corresponding nodes for this edge: '" + connectionID + "'");
			}
		});

		$(peopleData).remove();
		$(graphData).remove();
		
		
		Model.currentNode = Model.nodes[0];
		Model.previousNode = Model.currentNode;
		
		if(debug){
			console.log("loaded " + Model.people.length + " people into Model");
			console.log("loaded " + Model.nodes.length + " nodes into Model");
			console.log("loaded " + Model.connections.length + " connections into Model");
		}
    },
 	 onGUIchanged: function(){
		//TODO: have separate events for separate controls, or ask what changed. otherwise, redrawing all circles when changing path witdth.
		// for(var i = 0; i < GraphView.nodeViews.length; i++){
		// 	GraphView.nodeViews[i].particle.scale.set( GraphView.radius, GraphView.radius, 1);//TODO: add setRadius function (don't access all this directly)			
		// }

		//TODO: set text pos
		GraphView.textMesh.geometry.verticesNeedUpdate = true;
		

		// GraphView.topicText.attr('font-size', GraphView.font_size + "px");
		// GraphView.topicText.attr('x', GraphView.currNodeView.x + GraphView.topic_x);
		// GraphView.topicText.attr('y', GraphView.currNodeView.y + GraphView.topic_y);
	},
	loadData : function(){
		$.when( loader.loadXML("xml/people.xml"), loader.loadXML("xml/2013_7_25_Clouds_conversation.svg") ,loader.loadXML("xml/projects.xml")).done(function(peopleData, graphData, projectsData){
			Controller.setup(peopleData, graphData, projectsData);
			Controller.loop();
		});
			
	}
	
	
}

Controller.loadData();

