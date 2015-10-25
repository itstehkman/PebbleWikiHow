var express = require('express');
var app = express();
var bodyParser = require('body-parser');
var jsonParser = bodyParser.json();

var port = 80;

app.get('/get', function(req, res) {
	console.log('Get endpoint');
	var query = req.query.q;
	console.log('Got query ' + query);
	console.log('Spawning wikiHow.py');
	var spawn = require("child_process").spawn;	
	var process = spawn('python', ["scripts/wikiHow.py", query]);
	process.stdout.on('data', function(data) {
		console.log("[wikiHow.py stdout] " + data.toString());
		if (/^\s*$/.test(data.toString())) {
			res.setHeader('Content-Type', 'text/html');
			res.send('Error');
		} else {
			res.setHeader('Content-Type', 'application/json');
			res.send(data);
		}
	});
	process.stderr.on('data', function(data) {
		console.log("[wikiHow.py stderr] " + data.toString());
	});
});

app.listen(port, function() {
	console.log('Express server listening on port ' + port);
});
