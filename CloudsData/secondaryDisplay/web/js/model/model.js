Model = {
	nodes: new Array(),
	people: new Array(),
	projects: new Array(),
	connections: new Array(),
	previousNode: null,
	currentNode: null,
	topicChanged: false,
	topic: '.',
	addProject: function(project) {
		this.projects.push(project);
	},
	addPerson: function(person) {
		this.people.push(person);
		// if(debug){
			console.log("first: " + person.first 
						+ ", last: " + person.last 
						+ ", title: " + person.title 
						+ ", location: " + person.location 
						+ ", byline: " + person.byline1 
						+ ", description: " + person.description 
				);
		// }
	},
	addNode: function(node) {
		this.nodes.push(node);
		/* //CONNECT PEOPLE DATA TO CLIPS
		var personInNode = null;
		var found = false;		
		for (var i=0; i<this.people.length && !found; i++){

			if(n > 0){
				personInNode = this.people[i];
				found = true;
			}
		}
		if (personInNode!=null) personInNode.addNode(node);
		*/
	},
	addConnection: function(nodeA, nodeB, original_curve) {
		this.connections.push(new Connection(nodeA, nodeB, original_curve));
	},
	getProjectExample: function(project_example_title){
		var project = null;
		for(var i=0; i <this.projects.length && project==null; i++){
			if(this.projects[i].title == project_example_title){
				project = this.projects[i];
			}
		}
		return project;
	},
	getPerson: function(id){
		var person = null;
		for(var i=0; i <this.people.length && person==null; i++){
			if(this.people[i].first == id){
				person = this.people[i];
			}
			if(debug){
				console.log("first: " + this.people[i].first 
						+ ", last: " + this.people[i].last 
						+ ", title: " + this.people[i].title 
						+ ", location: " + this.people[i].location 
						+ ", byline: " + this.people[i].byline1 
						+ ", description: " + this.people[i].description 
				);
			}
			
		}
		return person;
	},	
	getNode: function(id){
		var node = null;
		for(var i=0; i <this.nodes.length && node==null; i++){
			if(this.nodes[i].id == id){
				node = this.nodes[i];
			}
		}
		return node;
	},
	setProjectExample: function(project_example_title){
		this.projectExample = this.getProjectExample(project_example_title);
	},
	setPerson: function (id) {
		this.currentPerson = this.getPerson(id);
		if(debug){
			console.log("first: " + this.currentPerson.first 
						+ ", last: " + this.currentPerson.last 
						+ ", title: " + this.currentPerson.title 
						+ ", location: " + this.currentPerson.location 
						+ ", byline: " + this.currentPerson.byline1 
						+ ", description: " + this.currentPerson.description 
				);
		}

	},
	setNode: function (id) {
		this.previousNode = this.currentNode;
		this.currentNode = this.getNode(id);
		if(this.currentNode != null) this.currentNode.markAsVisited();
		//this.currentNode.markAsVisited();
		if(debug){
			console.log("current node: " + this.currentNode.id);
		}
	},
	setTopic: function(topic){
		this.topic = topic;
	},	
	setQuestion: function(question){
		this.question = question;
	}
	
	
}
