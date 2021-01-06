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

		// Meas
		updateTable('.table2', [
			{name: 'u', val: x.getUint32(2, true) / 1000000.0 + ' V'},
			{name: 'i', val: x.getUint32(6, true) / 1000000.0 + ' A'},
			{name: 'power', val: x.getUint32(10, true) / 1000.0 + ' Wt'},
			{name: 'resistance', val: x.getUint32(14, true) / 1000.0 + ' Ohm'},
			{name: 'time', val: x.getUint32(18, true) + ' s'},
			{name: 'capacity', val: x.getUint32(22, true) / 1000.0 + ' Ah'},
			{name: 'uin', val: x.getUint32(26, true) / 1000000.0 + ' V'},
			{name: 'temperature', val: x.getUint16(30, true) / 10.0 + ' °C'}
		]);

		// State
		var state = x.getUint16(32, true);
		updateTable('.table1', [
			{name: 'overcurrent', val: (state & 1) ? '<p style="color:red;">YES</p>' : 'NO'},
			{name: 'switch', val: (enable != 0) ? '<p style="color:red;">ON</p>' : 'OFF'},
			{name: 'output', val: (state & 2) ? 'CC' : 'CV'}
		]);

		// Settings
		updateTable('.table3', [
			{name: 'u', val: x.getUint32(34, true) / 1000000.0 + ' V'},
			{name: 'i', val: x.getUint32(38, true) / 1000000.0 + ' A'},
		]);
	};
	oReq.send(null);
}
updateInfo();
setInterval(updateInfo, 500);
