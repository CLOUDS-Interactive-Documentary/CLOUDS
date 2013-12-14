Model = {
	clips: new Array(),
	people: new Array(),
	projects: new Array(),
	connections: new Array(),
	previousClip: null,
	currentClip: null,
	topic: "",
	projectExample: "",
	addProject: function(project) {
		this.projects.push(project);
	},
	addPerson: function(person) {
		this.people.push(person);
	},
	addClip: function(clip) {
		this.clips.push(clip);
		/* //CONNECT PEOPLE DATA TO CLIPS
		var personInClip = null;
		var found = false;		
		for (var i=0; i<this.people.length && !found; i++){
			var n = clip.id.search(this.people[i].first); //TODO: this is a hack while I don't have the new SVG format –people should have an ID (in people.xml), which should be included in the SVG. 
			if(n > 0){
				personInClip = this.people[i];
				found = true;
			}
		}
		if (personInClip!=null) personInClip.addClip(clip);
		*/
	},
	addConnection: function(clipA, clipB, original_curve) {
		this.connections.push(new Connection(clipA, clipB, original_curve));
	},
	getPerson: function(id){
		var person = null;
		for(var i=0; i <this.people.length && person==null; i++){
			if(this.people[i].first == id){
				person = this.people[i];
			}
		}
		return person;
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
	getClip: function(id){
		var clip = null;
		for(var i=0; i <this.clips.length && clip==null; i++){
			if(this.clips[i].id == id){
				clip = this.clips[i];
			}
		}
		return clip;
	},
	setPerson: function (id) {
		this.currentPerson = this.getPerson(id);
		if(debug){
			console.log("current person: " + this.currentPerson.first);
		}

	},
	setTopic: function(topic){
		this.topic = topic;
	},
	setProjectExample: function(project_example_title){
		this.projectExample = this.getProjectExample(project_example_title);
	},
	setClip: function (id) {
		this.previousClip = this.currentClip;
		this.currentClip = this.getClip(id);
		if(this.currentClip != null) this.currentClip.markAsVisited();
		//this.currentClip.markAsVisited();
		if(debug){
			console.log("current clip: " + this.currentClip.id);
		}
	}
	
}
