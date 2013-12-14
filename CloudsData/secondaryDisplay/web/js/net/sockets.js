sockets = {
	setup: function(clipMessageCallback){
		// setup websocket
		// get_appropriate_ws_url is a nifty function by the libwebsockets people
		// it decides what the websocket url is based on the broswer url
		// e.g. https://mygreathost:9099 = wss://mygreathost:9099
		//socket = new WebSocket(get_appropriate_ws_url());
		socket = new WebSocket(get_appropriate_ws_url(), "of-protocol");
		this.clipMessageCallback = clipMessageCallback;
		
		try {
			socket.onopen = this.onOpen;
			socket.onmessage = this.onMessage;
			socket.onclose = this.onClose;
			
		} catch(exception) {
			alert('<p>Error' + exception);  
		}
	},
	onOpen: function(){
		if(debug){
			console.log("web socket connection open");
		}
	},
	onMessage: function(msg){
		var message = JSON.parse(msg.data);
		if(debug){
			console.log("received socket message: " + message);
		}

		//if (message.clip){
			 sockets.clipMessageCallback(message);
		//}
	},
	onClose: function(){
		if(debug){
			console.log("web socket connection closed");
		}
	}

}