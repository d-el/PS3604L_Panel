function nano(template, data){
	return template.replace(/\{([\w\.]*)\}/g, function(str, key){
	var keys = key.split("."), v = data[keys.shift()];
	for (var i = 0, l = keys.length; i < l; i++) v = v[keys[i]];
	return (typeof v !== "undefined" && v !== null) ? v : "";
	});
}

function updateTable(query, data){
	var table = document.querySelector(query);
	table.innerHTML = '';
	for(var key in data){
		table.innerHTML += nano('<tr><td>{name}</td><td>{val}</td></tr>', data[key]);
	}
}

let getVersion = true;

function updateInfo(){
	if(getVersion){
		var vReq = new XMLHttpRequest();
		vReq.open('GET', 'version.bin', true);
		vReq.timeout = 10000;
		vReq.responseType = 'arraybuffer';
		vReq.onload = function (oEvent){
			if(vReq.status == 200){
				getVersion = false;
				var x = new DataView(vReq.response, 0);
				pmajor = x.getUint16(0, true);
				pminor = x.getUint16(2, true);
				ppatch = x.getUint16(4, true);
				rmajor = x.getUint16(6, true);
				rminor = x.getUint16(8, true);
				rpatch = x.getUint16(10, true);
				document.querySelector('footer').innerHTML =
					'©copyright: 2013-2026 DEL	P' + pmajor + '.' + pminor + '.' + ppatch + ' : R' + rmajor + '.' + rminor + '.' + rpatch;
			}
		}
		vReq.send(null);
	}

	var oReq = new XMLHttpRequest();
	oReq.open('GET', 'statemeastask.bin', true);
	oReq.timeout = 5000;
	oReq.responseType = 'arraybuffer';
	
	oReq.ontimeout = function (oEvent){
		var statusbox = document.querySelector('.statusbox');
		if(statusbox.classList.contains('hide')){
			statusbox.style.background = 'red';
			statusbox.innerHTML = 'Connection timeout';
			statusbox.classList.remove('hide');
		}
	}
	
	oReq.onload = function (oEvent){
		if(oReq.status == 200){
			var statusbox = document.querySelector('.statusbox');
			if(!statusbox.classList.contains('hide')){
				statusbox.style.background = '#398a1e';
				statusbox.style.display = 'block';
				statusbox.innerHTML = 'Connection successful';
				setTimeout(function(){
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

		// Meas
		resistance_raw = x.getInt32(14, true)
		resistance = resistance_raw > 0 ? resistance_raw / 10000.0 : '---'
		updateTable('.table2', [
			{name: 'u', val: round(x.getInt32(2, true) / 1000000.0, 3) + ' V'},
			{name: 'i', val: round(x.getInt32(6, true) / 1000000.0, 6) + ' A'},
			{name: 'power', val: x.getUint32(10, true) / 1000000.0 + ' W'},
			{name: 'resistance', val: resistance + ' Ohm'},
			{name: 'time', val: x.getUint32(18, true) / 1000.0 + ' s'},
			{name: 'capacity', val: x.getUint32(22, true) / 1000.0 + ' Ah'},
			{name: 'temperature', val: x.getInt16(30, true) / 10.0 + ' °C'}
		]);

		// Status
		var status = x.getUint16(32, true);
		var error = x.getUint16(34, true);
		var disablecause = x.getUint16(36, true);
		let disablecause2string = new Map([
			[0, 'none'],
			[1, 'Error Temperature Sensor'],
			[2, 'Overheated'],
			[3, 'Low Input Voltage'],
			[4, 'Reserved'],
			[5, 'OverCurrent'],
			[6, 'Time Shutdown'],
			[7, 'Low Current Shutdown'],
			[8, 'Request']
		]);
		updateTable('.table1', [
			{name: 'mode', val: (status & 1) ? 'CC' : 'CV'},
			{name: 'error', val: (error & 1) ? 'temperatureSensor' : '-'},
			{name: 'disablecause', val: disablecause2string.get(disablecause)},
			{name: 'output', val: (enable != 0) ? '<p style="color:red;">ON</p>' : 'OFF'}
		]);

		// Settings
		updateTable('.table3', [
			{name: 'u', val: x.getUint32(38, true) / 1000000.0 + ' V'},
			{name: 'i', val: x.getUint32(42, true) / 1000000.0 + ' A'},
		]);
	};
	oReq.send(null);
}

updateInfo();
setInterval(updateInfo, 500);
