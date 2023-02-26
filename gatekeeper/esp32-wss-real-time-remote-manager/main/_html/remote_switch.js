/**************************************************************/
/* Remote Switch JS code by HA3FLT, HA2OS @ 2022-2023         */
/*                                                            */
/* License: GNU AGPLv3                                        */
/**************************************************************/

// Start up
function StartFunction()
{
var loc = window.location, new_uri;
	if (loc.protocol === "https:") {
		new_uri = "wss:";
	} else {
		new_uri = "ws:";
	}
	new_uri += "//" + loc.host;
	new_uri += loc.pathname + "ws";

	document.getElementById("wsServer").value = new_uri;
	ConnectRem();
}

// Communication
var ws;

function ConnectRem()
{
	ConnectRemoteSw();
}

function ConnectRemoteSw()
{
	if ("WebSocket" in window) {
		ws = new WebSocket(document.getElementById("wsServer").value);
	} else {
		// The browser doesn't support WebSocket
		alert("WebSocket NOT supported by your Browser!");
	}
	ws.binarytype = "arraybuffer";
	ws.onopen = function(evt) { onOpen(evt) };
	ws.onclose = function(evt) { onClose(evt) };
	ws.onmessage = function(evt) { onMessage(evt) };
	ws.onerror = function(evt) { onError(evt) };

	// http://stackoverflow.com/questions/4812686/closing-websocket-correctly-html5-javascript
	/*window.onbeforeunload = function() {
		ws.onclose = function() {};
		DisconnectRemoteSw();
	};*/
}

function DisconnectRem()
{
	DisconnectRemoteSw(ws);
}

function onOpen(evt)
{
	console.log("Connected");
	SetRemoteConnection(true);
	//ws.send("\n");
}
	
function onClose(evt)
{
	SetRemoteConnection(false);
	console.log("Websocket closed");
}

function onMessage(evt)
{
	//console.log("Message received: " + evt.data);
	let msg = evt.data;

	addText(msg);
    // TODO: Not OK to manually append instead of printing what we received
	addNewline();
	termScrollUp();

	/*if (msg.length == 3 && msg[0] == 'O' && msg[1] == 'K' && msg[2] == '>') {		// "OK>"
		console.log("Prompt read, getting switches");
		GetSwitches();
	}
	else if (msg.length == 10 && msg[0] == 'G' && msg[9] == '>') {		// "gnnnnnnnn>"
		console.log("Got data, setting switches");
		ShowSwitches(msg);
	}*/
}

function onError(evt)
{
	let error = evt;
	console.log("WebSocket Error " + error.data);
}
	
function DisconnectRemoteSw()
{
	if (ws.readyState == ws.OPEN) {
		console.log("Websocket disconnecting...");
		ws.send("d\n");
		/*Sleep(100);
		ws.close();*/
	}
	else {
		Beep();
	}
}

function GetSwitches()
{
	if (ws.readyState == ws.OPEN) {
		ws.send("g\n");
	}
	else {
		Beep();
	}
}

function ShowSwitches(nSwitches)
{
	for (i = 1; i < 8; i++) {
		document.getElementById("switch" + i).checked = (nSwitches[i] != '0');
	}
}

// Utilities
function Beep()
{
	let snd = new Audio("data:audio/wav;base64,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qJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=");
	snd.play();
}

// UI functions
function RemoteConnection()
{
	if (ws.readyState == ws.CLOSED) {
		ConnectRem();
	}
	else {
		DisconnectRem();
	}
}

function SetRemoteConnection(bConnection)
{
	let ConnBtnText = document.getElementById("RemoteConnection").firstChild;
	if (bConnection) {
		ConnBtnText.data = "Disconnect";
	}
	else {
		ConnBtnText.data = "Connect";
	}
}

function SetSwitch(sSwitch)
{
	if (ws == null) {
		Beep();
		return;
	}
	if (ws.readyState == ws.OPEN) {
		if (document.getElementById(sSwitch).checked) {
			switch (sSwitch) {
				case "switch1":	ws.send("!\n"); break;
				case "switch2":	ws.send("@\n"); break;
				case "switch3":	ws.send("#\n"); break;
				case "switch4":	ws.send("$\n"); break;
				case "switch5":	ws.send("%\n"); break;
				case "switch6":	ws.send("^\n"); break;
				case "switch7":	ws.send("&\n"); break;
				case "switch8":	ws.send("*\n"); break;
			}
		}
		else {
			switch (sSwitch) {
				case "switch1":	ws.send("1\n"); break;
				case "switch2":	ws.send("2\n"); break;
				case "switch3":	ws.send("3\n"); break;
				case "switch4":	ws.send("4\n"); break;
				case "switch5":	ws.send("5\n"); break;
				case "switch6":	ws.send("6\n"); break;
				case "switch7":	ws.send("7\n"); break;
				case "switch8":	ws.send("8\n"); break;
			}
		}
	}
	else {
		Beep();
	}
}

// Terminal
function addText(text)
{
	const textNode = document.createTextNode(text);
	let wrapper = document.getElementById('term_wrapper');
	wrapper.appendChild(textNode);
}

function addNewline()
{
	let br = document.createElement('br');
	let wrapper = document.getElementById('term_wrapper');
	wrapper.appendChild(br);
}

function termSendKeyup(event)
{
	if (event.keyCode === 13) {
		let button = document.getElementById('btn_submit');
		button.click();
	}
}

function termScrollUp()
{
	let wrapper = document.getElementById('term_wrapper');
	wrapper.scrollTop = wrapper.scrollHeight /*- wrapper.clientHeight*/;
}

function termSend()
{
	let input = document.getElementById('input_source');
	let value = input.value;

	ws.send(value + '\r\n');

	/*addText(value);
	addNewline();
	termScrollUp();*/

	input.value = '';
}

/* End */
