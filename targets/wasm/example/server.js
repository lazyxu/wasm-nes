var express = require('express');
var app = express();

express.static.mime.types['wasm'] = 'application/wasm'
express.static.mime.types['nes'] = 'application/octet-stream'

app.use(express.static('./static'));
app.use("/roms", express.static("../../../tests"));

var server = app.listen(1111, function () {
  var port = server.address().port;
  console.log("http://localhost:" + port);
});