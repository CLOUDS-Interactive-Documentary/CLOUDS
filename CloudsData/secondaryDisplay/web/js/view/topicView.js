TopicView = {	
	init:function() {
		
	},
	update: function(){
		console.log("topic. " + Model.topic);
		$('#topic').html(Model.topic);
	}
}
