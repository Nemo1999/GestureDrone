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
      .listen( 3000,"0.0.0.0", ()=>{console.log('https server listening on 0.0.0.0 , port 3000')})

app.get('/', (req, res) => {
  //res.sendFile(__dirname+'/index.html');
  res.sendFile(__dirname+'/visualization.html');
});

app.get('/script_for_aframe.js', (req, res) => {
  res.sendFile(__dirname+'/script_for_aframe.js');
});

app.get('/script_for_visualization.js', (req, res) => {
  res.sendFile(__dirname+'/script_for_visualization.js');
});

app.get('/kinematic-body.js', (req, res) => {
  res.sendFile(__dirname+'/kinematic-body.js');
});


//--------------- socket ----------------
//open socket server on port 3000
const io = require('socket.io')(server)
io.on('connection', socket => {
  console.log('socket  connected!');
})


// read osc input
let gyro = {a:0,b:0,c:0}

var osc = require("osc");
var getIPAddresses = function () {
  var os = require("os"),
      interfaces = os.networkInterfaces(),
      ipAddresses = [];

  for (var deviceName in interfaces) {
      var addresses = interfaces[deviceName];
      for (var i = 0; i < addresses.length; i++) {
          var addressInfo = addresses[i];
          if (addressInfo.family === "IPv4" && !addressInfo.internal) {
              ipAddresses.push(addressInfo.address);
          }
      }
  }

  return ipAddresses;
};

var udpPort = new osc.UDPPort({
  localAddress: "0.0.0.0",
  localPort: 57121
});

udpPort.on("ready", function () {
  var ipAddresses = getIPAddresses();

  console.log("Listening for OSC over UDP.");
  ipAddresses.forEach(function (address) {
      console.log(" Host:", address + ", Port:", udpPort.options.localPort);
  });
});

udpPort.on("message", function (oscMessage) {
  console.log(oscMessage);
  switch(oscMessage.address){
    case '/orientation/alpha':
      gyro.a = oscMessage.args[0]
    break;
    case '/orientation/beta':
      gyro.b = oscMessage.args[0]
    break;
    case '/orientation/gamma':
      gyro.c = oscMessage.args[0]
    break;
    default:
  }
});

udpPort.on("error", function (err) {
  console.log(err);
});

udpPort.open();

setInterval(()=>{io.sockets.emit('update_data',gyro)},50);

//------ read from sensor input ----------

/*
const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')


x = 0
y = 0
z = 0
h = 0
var joystick_time = "0";
var date = new Date();
var sensor_1="";
var sensor_2="";
var sensor_3="";   

// open serial port
const port = new SerialPort('/dev/ttyUSB0', { baudRate: 9600  })
//console.log(port)

// feed port input to a line parser
const parser = port.pipe(new Readline({delimiter: '\n'} ))
// when parser get a line , send it to the socket
parser.on('data', line => {
    //var data = JSON.parse(line);
    //sensor_data = data.gyro2.x+","+data.gyro2.y+","+data.gyro2.z+","+data.acc2.x+","+data.acc2.y+","+data.acc2.z+",";
    io.sockets.emit('update_data',line);
    h = date.getUTCHours();
    m = date.getUTCMinutes();
    s = date.getUTCSeconds();
    var sensor_time = h+":"+m+":"+s;
    parsed_data = line.split(",");
    if(parsed_data[0]==1)
      sensor_1 = line
    if(parsed_data[0]==2)
      sensor_2 = line
    if(parsed_data[0]==3)
      sensor_3 = line
    //sensor_data = sensor_time+","+line;
    console.log("Serial Input: ",line);
    if(collect_data)
    {
      //data = data + sensor_time+","+sensor_1+","+sensor_2+","+sensor_3+","+joystick_time+","+ x+","+y+","+z+"\n";
      console.log(sensor_time+","+sensor_1+","+sensor_2+","+sensor_3+","+joystick_time+","+ x+","+y+","+z+"\n");
    }
    //console.log(line);
  }
)

*/
/*
//------ read from sensor joystick --------
var gamepad = require("gamepad")
gamepad.init()
setInterval(gamepad.processEvents, 10);
var collect_data = false;
var data = "";
gamepad.on("up", function (id, num) {
      if(num==4){
        collect_data = true;
        console.log("record start");
      }
      if(num==5)
      {
        collect_data = false;
        console.log("record finished");
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
    console.log(x,y,z,h);
    h = date.getUTCHours();
    m = date.getUTCMinutes();
    s = date.getUTCSeconds();
    joystick_time = h+":"+m+":"+s;

});
*/
fs = require('fs')
record_data= function(data){
    l = data.split("\r\n|\r|\n").length;     
    console.log(l + "entries of data collected");
    fs.writeFile("training_data.txt",data,function(err){});
    console.log('file saved as "traning_data.txt"');
};


    


