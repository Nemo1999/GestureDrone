let serial;
let Wx = 0, Wy = 0 , Wz = 0;
 
function preload() {
  inconsolata = loadFont('Inconsolata.otf');
}


function setup() {
  createCanvas(windowWidth, windowHeight, WEBGL)
  textFont(inconsolata);
  textSize(width / 50)
  textAlign(CENTER, CENTER);
  // Instantiate our SerialPort object
  serial = new p5.SerialPort();
 
  // Let's list the ports available
  let portlist = serial.list();
 
  // Assuming our Arduino is connected, let's open the connection to it
  // Change this to the name of your arduino's serial port
  serial.open("/dev/ttyUSB0");
 
  // Register some callbacks
 
  // When we connect to the underlying server
  serial.on('connected', serverConnected);
 
  // When we get a list of serial ports that are available
  serial.on('list', gotList);
 
  // When we some data from the serial port
  serial.on('data', gotData);
 
  // When or if we get an error
  serial.on('error', gotError);
 
  // When our serial port is opened and ready for read/write
  serial.on('open', gotOpen);
}
 
// We are connected and ready to go
function serverConnected() {
    print("We are connected!");
}
 
// Got the list of ports
function gotList(thelist) {
  // theList is an array of their names
  for (let i = 0; i < thelist.length; i++) {
    // Display in the console
    print(i + " " + thelist[i]);
  }
}
 
// Connected to our serial device
function gotOpen() {
  print("Serial Port is open!");
}
 
// Ut oh, here is an error, let's log it
function gotError(theerror) {
  print(theerror);
}
 
// There is data available to work with from the serial port
function gotData() {
  let currentString = serial.readStringUntil("\r\n");
  if(currentString != ""){
  //console.log( currentString);
  let strs = currentString.split(',');
  Wx = float(strs[0])*PI;
  Wy = float(strs[1])*PI;
  Wz = float(strs[2])*PI;
  }
}
 
// Methods available
// serial.read() returns a single byte of data (first in the buffer)
// serial.readChar() returns a single char 'A', 'a'
// serial.readBytes() returns all of the data available as an array of bytes
// serial.readBytesUntil('\n') returns all of the data available until a '\n' (line break) is encountered
// serial.readString() retunrs all of the data available as a string
// serial.readStringUntil('\n') returns all of the data available as a tring until a (line break) is encountered
// serial.last() returns the last byte of data from the buffer
// serial.lastChar() returns the last byte of data from the buffer as a char
// serial.clear() clears the underlying serial buffer
// serial.available() returns the number of bytes available in the buffer
 
function draw() {
  // Polling method
  sx = sin(Wy); cx = cos(Wy);
  sy = sin(Wz); cy = cos(Wz);
  sz = sin(Wx); cz = cos(Wx);
  background(250);
  normalMaterial();
  push();
  rotateX(Wy);
  rotateY(Wz);
  rotateZ(Wx);
   /* applyMatrix(cz*cy, cz*sy*sx-sz*cx ,  cz*sy*cx+sz*sx , 0.0,
                sz*cy, sz*sy*sx+cz*cx ,  sz*sy*cx-cz*sx, 0.0,
                -sy  ,          cy*sx ,  cy*cx        , 0.0 ,
                  0.0, 0.0, 0.0,  1.0);
  */
  box(width/7 , height/7);
  pop();
  fill('black');
  
  text('Wx :'+str(Wx).slice(0,5) , width/5,height/5);
  text('Wy :'+str(Wy).slice(0,5) , width/5,height/10);
  text('Wz :'+str(Wz).slice(0,5) , width/5,0);
/*
  /*

/*
  if (serial.available() > 0) {
    let data = serial.read();
    ellipse(50,50,data,data);
  }
*/
}
