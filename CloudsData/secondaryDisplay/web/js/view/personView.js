PersonView = {	
	init:function() {
		
	},
	update: function(){
		var person = Model.currentPerson;
		$('#first').html(person.first);
		$('#last').html(person.last);
		$('#title').html(person.title);
		$('#location').html(person.location);
		$('#byline').html(person.byline1);
		$('#description').html(person.byline2);
	}
}
