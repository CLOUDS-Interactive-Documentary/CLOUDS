ProjectExampleView = {	

	init:function() {
	// 	$('#video video').bind('ended', function(){
 	//	$(this).parent().fadeOut()
	// })
	},
	update: function(){
		var project = Model.projectExample;
		$('#projectex_title').html(project.title);
		$('#projectex_artist').html(project.creatorName);
		$('#projectex_desc').html(project.description);
		$('#video').attr("source", project.video_file_name);
		$('#video').attr("src", project.videoFileName);
		$('#video').load();

		console.log($('#video').attr("src"));
	},
	
}