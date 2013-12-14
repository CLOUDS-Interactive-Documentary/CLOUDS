ProjectExampleView = {	

	init:function() {
	 	$('#video').bind('ended', function(){
 		Controller.setMode(Controller.modes.GRAPH);
	 	})
	},
	update: function(){
		var project = Model.projectExample;
		$('#projectex_title').html(project.title);
		$('#projectex_artist').html(project.creatorName);
		$('#projectex_desc').html(project.description);
		$('#video').attr("source", project.video_file_name);
		$('#video').attr("src", project.videoFileName);
		$('#video').load();
		
	},
	
}