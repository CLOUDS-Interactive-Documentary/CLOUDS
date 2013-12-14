PersonView = {	
	init:function() {
		
	},
	update: function(){
		var person = Model.currentPerson;
		$('#artist_name').html(person.first + ' ' + person.last);
		$('#byline').html(person.byline1);
		$('#description').html(person.byline2);
		
	}
}
