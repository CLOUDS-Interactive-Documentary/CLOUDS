loader = {
	loadXML: function(filePath) {
		return $.ajax({
				type: "GET",
				url: filePath,
				dataType: "xml"
				// ,
				// success: function (response) {
		  //       	console.log(response);
    //   			},
				// error: function (xhr, ajaxOptions, thrownError) {
    //     			console.log(thrownError);
    //   			}
			});
	}
	
}



