const express = require('express')
const app = express()

//open express server on port 3000
const server = require('http').Server(app)
    .listen(3000,()=>{console.log('open server!')})

//open socket server on port 3000
const io = require('socket.io')(server)

io.on('connection', socket => {
  console.log('success connect!');
})


const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
// open serial port
const port = new SerialPort('/dev/ttyUSB0', { baudRate: 9600  })
// feed port input to a line parser
const parser = port.pipe(new Readline({delimiter: '\n'} ))
// when parser get a line , send it to the socket
parser.on('data', line => {  
    io.sockets.emit('update_data', line);
    console.log(line);
})
