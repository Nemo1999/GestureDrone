let Sensors = {humd:80,gyro:{x:0,y:0,z:0},acc:{x:0,y:0,z:0},temp:28};
let reset = false;
let BiasAng = {Bx:0,By:0,Bz:0};
let angY = 0;

      //reset the bias angle with enter key.
const handleKeyDown = e =>
{
	if (e.key === 'Enter')
	{
	    reset = true;
	    //console.log('reset');
	}
}

window.addEventListener("keydown",handleKeyDown);

//------------ socket for sensor------------
var socket = io();


socket.on("update_data", (data) =>
{
	let  diff
	//console.log(data);
	try {
	    diff = JSON.parse(data);
	}
	catch(e){
	    console.log("parse error");
	}
	Sensors =  ({...Sensors,...diff});
	//console.log(diff);
	//console.log(Sensors);
	//updateView();
	if(reset){
	    BiasAng = {
		Bx: Sensors.gyro2.x,
		By: Sensors.gyro2.y,
		Bz: Sensors.gyro2.z
	    }
	    reset = false;

	}
});


//------------ A-frame ------------
//const entityEl = document.querySelector("a-entity");
const sceneEl = document.querySelector("a-scene");
const translateController = sceneEl.querySelector("#translate-controller");
const rotateController = sceneEl.querySelector("#rotate-controller");
const rotateY = sceneEl.querySelector("#rotateY");
const textController = sceneEl.querySelector("#text");

//const viewController = entityEl.querySelector("#camera");
//const box = sceneEl.querySelector("#box");


const updateView = () => {

	  //console.log(textController.getAttribute("text"));
	  //console.log()
	  //textController.setAttribute("text",`value:${"Temp:"+ Math.round(Sensors.temp * 100)/100+"\nHumid:"+(Math.round(Sensors.humd*100)/100)};color:#EF2D5E`);

	  const Wx = Sensors.gyro2.x - BiasAng.Bx;
	  const Wy = Sensors.gyro2.y - BiasAng.By;
	  const Wz = Sensors.gyro2.z - BiasAng.Bz;
	  //const rot = rotateController.getAttribute("rotation");
	  //console.log([rot.x,rot.y,rot.z]);

	  ///*============= for sensor ==============
	  if(Wz>10)
	      angY += 2;
	  else if (Wz < -10)
	      angY -= 2;
	  else
	      ;
      //=========================================*/

	  //angY = -\Wz;
      let angle_panelty = 0.5;
	  rotateY.setAttribute("rotation",{x:0,y:angle_panelty*angY,z:0});
	  rotateController.setAttribute("rotation", { x: angle_panelty*Wx, y:0 , z: -(Wy*angle_panelty) });
	  // box.setAttribute("rotation", { x: Wx, y: Wy, z: Wz });
	  const { x, y, z } = translateController.getAttribute("position");
	  //console.log([x,y,z]);
	  let  dx1 = 0;
	  let  dy1 = 0;
	  let  dx2 = 0;
	  let  dy2 = 0;
	  let strip=5;
	  if(Math.abs(Wy)>10){
	      dx1 = strip*Math.cos(angY/180*Math.PI)*0.05 * Math.sign(Wy)*Math.min(3,Math.abs(Wy));
	      dy1 = strip*Math.sin(angY/180*Math.PI)*0.05 * Math.sign(Wy)*Math.min(3,Math.abs(Wy));
	  }
	  if(Math.abs(Wx)>10){
	      dx2 = strip*Math.cos(angY/180*Math.PI+Math.PI/2)*0.05 * Math.sign(Wx)*Math.min(3,Math.abs(Wx));
	      dy2 = strip*Math.sin(angY/180*Math.PI+Math.PI/2)*0.05 * Math.sign(Wx)*Math.min(3,Math.abs(Wx));
	  }
	  translateController.setAttribute("position", {
	      x: x + dx1 + dx2,
	      y: -2.5,//-Pz
	      z: z + dy1 + dy2
	  });
};


setInterval(updateView, 100);
