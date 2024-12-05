function nano(template, data) {
	return template.replace(/\{([\w\.]*)\}/g, function(str, key) {
	var keys = key.split("."), v = data[keys.shift()];
	for (var i = 0, l = keys.length; i < l; i++) v = v[keys[i]];
	return (typeof v !== "undefined" && v !== null) ? v : "";
	});
}

function updateTable(query, data) {
	var table = document.querySelector(query);
	table.innerHTML = '';
	for(var key in data) {
		table.innerHTML += nano('<tr><td>{name}</td><td>{val}</td></tr>', data[key]);
	}
}

function updateInfo() {

	var oReq = new XMLHttpRequest();
	oReq.open('GET', 'statemeastask.bin', true);
	oReq.timeout = 5000;
	oReq.responseType = 'arraybuffer';
	
	oReq.ontimeout = function (oEvent) {
		var statusbox = document.querySelector('.statusbox');
		if(statusbox.classList.contains('hide')){
			statusbox.style.background = 'red';
			statusbox.innerHTML = 'Connection timeout';
			statusbox.classList.remove('hide');
		}
	}
	
	oReq.onload = function (oEvent) {
		if(oReq.status == 200){
			var statusbox = document.querySelector('.statusbox');
			if(!statusbox.classList.contains('hide')){
				statusbox.style.background = '#398a1e';
				statusbox.style.display = 'block';
				statusbox.innerHTML = 'Connection successful';
				setTimeout(function() {
					statusbox.classList.add('hide');
				}, 5000);
			}
		}

		var x = new DataView(oReq.response, 0);

		// Connect
		var connect = x.getUint8(0, true);

		// Enable
		var enable = x.getUint8(1, true);

		function round(x, dig) { return Number.parseFloat(x).toFixed(dig); }

		// Version
		major = x.getUint16(0, true);
		minor = x.getUint16(2, true);
		patch = x.getUint16(4, true);

		// Meas
		resistance_raw = x.getInt32(20, true)
		resistance = resistance_raw > 0 ? resistance_raw / 10000.0 : '---'
		updateTable('.table2', [
			{name: 'u', val: round(x.getInt32(8, true) / 1000000.0, 3) + ' V'},
			{name: 'i', val: round(x.getInt32(12, true) / 1000000.0, 6) + ' A'},
			{name: 'power', val: x.getUint32(16, true) / 1000.0 + ' W'},
			{name: 'resistance', val: resistance + ' Ohm'},
			{name: 'time', val: x.getUint32(24, true) / 1000.0 + ' s'},
			{name: 'capacity', val: x.getUint32(28, true) / 1000.0 + ' Ah'},
			{name: 'uin', val: round((x.getInt32(32, true) / 100000) / 10, 1) + ' V'},
			{name: 'temperature', val: x.getInt16(36, true) / 10.0 + ' °C'}
		]);

		// Status
		var status = x.getUint16(38, true);
		var disablecause = x.getUint16(40, true);
		let disablecause2string = new Map([
			[0, 'none'],
			[1, 'Error Temperature Sensor'],
			[2, 'Overheated'],
			[3, 'Low Input Voltage'],
			[4, 'Reverse Voltage'],
			[5, 'OverCurrent'],
			[6, 'Time Shutdown'],
			[7, 'Low Current Shutdown'],
			[8, 'Request']
		]);
		updateTable('.table1', [
			{name: 'output', val: (status & 64) ? 'CC' : 'CV'},
			{name: 'disablecause', val: disablecause2string.get(disablecause)},
			{name: 'switch', val: (enable != 0) ? '<p style="color:red;">ON</p>' : 'OFF'}
		]);

		// Settings
		updateTable('.table3', [
			{name: 'u', val: x.getUint32(42, true) / 1000000.0 + ' V'},
			{name: 'i', val: x.getUint32(46, true) / 1000000.0 + ' A'},
		]);

		document.querySelector('footer').innerHTML = '©copyright: 2013-2024 DEL	rv' + major + '.' + minor + '.' + patch;
	};
	oReq.send(null);
}
updateInfo();
setInterval(updateInfo, 500);
