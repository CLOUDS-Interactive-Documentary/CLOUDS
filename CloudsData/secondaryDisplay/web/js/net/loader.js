loader = {
	loadXML: function(filePath) {
		return $.ajax({
				type: "GET",
				url: filePath,
				dataType: "xml"
			});
	}
	
}



