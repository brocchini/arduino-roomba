	// Application object.
	var app = {}

	// Connected device.
	app.device = null;
	
	// Roomba Drive Commands
	app.drive = { 
	  stop:     new Uint8Array([0]),
	  forward:  new Uint8Array([1]),
	  backward: new Uint8Array([2]),
	  left:     new Uint8Array([3]),
	  right:    new Uint8Array([4]) 
	};
	
	app.writeToBle = function(msg)
	{
	    app.showMessage('writeToBle [' +msg[0]+']');
		app.device && app.device.writeDataArray(msg);
	}
	app.showMessage = function(info)
	{
		document.getElementById('info').innerHTML = info
	};

	// Called when BLE and other native functions are available.
	app.onDeviceReady = function()
	{
		app.showMessage('Touch the connect button to begin.');
	};

	app.connect = function()
	{
		evothings.arduinoble.close();

		evothings.arduinoble.connect(
			'arduinoble', // Name of BLE shield.
			function(device)
			{
				app.device = device;
				app.showMessage('Connected! Tap buttons to drive Roomba.');
			},
			function(errorCode)
			{
				app.showMessage('Connect error: ' + errorCode + '.');
			});
	};

