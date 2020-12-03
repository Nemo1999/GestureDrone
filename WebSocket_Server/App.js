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

app.get('/', (req, res) => {
  res.sendFile(__dirname+'/index.html');
});

app.get('/script_for_aframe.js', (req, res) => {
  res.sendFile(__dirname+'/script_for_aframe.js');
});

app.get('/kinematic-body.js', (req, res) => {
  res.sendFile(__dirname+'/kinematic-body.js');
});


//--------------- socket ----------------
//open socket server on port 3000
const io = require('socket.io')(server)
io.on('connection', socket => {
  console.log('success connect!');
})


//------ read from sensor input ----------




const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')

    
// open serial port

const port = new SerialPort('/dev/ttyUSB0', { baudRate: 9600  })
//console.log(port)

// feed port input to a line parser
const parser = port.pipe(new Readline({delimiter: '\n'} ))
// when parser get a line , send it to the socket
parser.on('data', line => {
    var data = JSON.parse(line);
    sensor_data = data.gyro2.x+","+data.gyro2.y+","+data.gyro2.z+","+data.acc2.x+","+data.acc2.y+","+data.acc2.z+",";
    //io.sockets.emit('update_data', line);
    console.log(line);
})




x = 0
y = 0
z = 0
h = 0



//------ read from sensor joystick --------
var gamepad = require("gamepad")
gamepad.init()
setInterval(gamepad.processEvents, 10000);
var collect_data = false;
var data = "";
gamepad.on("up", function (id, num) {
      if(num==4)
        collect_data = true;
      if(num==5)
      {
        collect_data = false;
        console.log("down pressed");
        record_data(data);
      }

});

// Listen for button down events on all gamepads
/*
gamepad.on("down", function () {

    collect_data = false;
    console.log("down pressed");
    record_data(data);
});
*/
gamepad.on("move", function (id, axis, value) {

    var str = "{\"gyro\":{\"x\":"+x+",\"y\":"+y+",\"z\":"+z+",\"h\":"+h+"}}"
    // 左邊搖桿左右
    if(axis==0)
    {
        z = value*90
        str = "{\"gyro\":{\"x\":"+x+",\"y\":"+y+",\"z\":"+z+",\"h\":"+h+"}}"
    }
    if(axis==1)
    {
        h = value*90
        str = "{\"gyro\":{\"x\":"+x+",\"y\":"+y+",\"z\":"+z+",\"h\":"+h+"}}"
    }
    // 右邊搖桿左右
    if(axis==3)
    {
        y = -value*90
        str = "{\"gyro\":{\"x\":"+x+",\"y\":"+y+",\"z\":"+z+",\"h\":"+h+"}}"
    }
    // 右邊搖桿上下
    if(axis==4)
    {
        x = -value*90
        str = "{\"gyro\":{\"x\":"+x+",\"y\":"+y+",\"z\":"+z+",\"h\":"+h+"}}"
    }
    io.sockets.emit('update_data', str);
    console.log(str);

    if(collect_data)
    {
        data = data + sensor_data+","+ x+","+y+","+z+"\n";
    }

});

fs = require('fs')
record_data= function(data){
    console.log("write_file")
    console.log(data)
    fs.writeFile("training_data.txt",data,function(err){});
};


    


