var path = require('path')
var fs = require('fs')
const express = require('express')
const app = express()

var certOptions = {
  key: fs.readFileSync(path.resolve('cert/server.key')),
  cert: fs.readFileSync(path.resolve('cert/server.crt'))
}

//open express server on port 3000
const http = require('https')

const server = http.createServer(certOptions ,app)
      .listen( 3000,"0.0.0.0", ()=>{console.log('listening on 3000')})



//open socket server on port 3000
const io = require('socket.io')(server)


app.get('/', (req, res) => {
  res.sendFile(__dirname+'/index.html');
});


io.on('connection', socket => {
  console.log('success connect!');
})


const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
// open serial port
const port = new SerialPort('/dev/input/js0', { baudRate: 9600  })
// feed port input to a line parser
const parser = port.pipe(new Readline({delimiter: '\n'} ))
// when parser get a line , send it to the socket
parser.on('data', line => {  
    io.sockets.emit('update_data', line);
    //console.log(line);
})
