let Sensors = {humd:80,gyro:{x:0,y:0,z:0,h:-5},acc:{x:0,y:0,z:0},temp:28};
let reset = false;
let BiasAng = {Bx:0,By:0,Bz:0};
let angY = 0;
let height = 0;
let gyroAng = {x:0,y:0,z:0};

//reset the bias angle with enter key.
const handleKeyDown = e =>
{
	if (e.key === 'Enter')
	{
	    reset = true;
	    //console.log('reset');
	}
	console.log(e.key)
}

window.addEventListener("keydown",handleKeyDown);


//------------ A-frame ------------
//const entityEl = document.querySelector("a-entity");
const sceneEl = document.querySelector("a-scene");
const translateController = sceneEl.querySelector("#translate-controller");
const rotateController = sceneEl.querySelector("#rotate-controller");
const rotateY = sceneEl.querySelector("#rotateY");
const textController = sceneEl.querySelector("#text");
const torsoController = sceneEl.querySelector("#torso-rotate");
//const viewController = entityEl.querySelector("#camera");
//const box = sceneEl.querySelector("#box");


//------------ socket for sensor------------
var socket = io();

const updateView = (gyro) => {

	//textController.setAttribute("text",`value:${"Temp:"+ Math.round(Sensors.temp * 100)/100+"\nHumid:"+(Math.round(Sensors.humd*100)/100)};color:#EF2D5E`);


	//rotateY.setAttribute("rotation",{x:0,y:angle_panelty*angY,z:0});//current
	//rotateController.setAttribute("rotation", { x: angle_panelty*Wx, y:0 , z: -(Wy*angle_panelty) });
	torsoController.setAttribute("rotation", { x: gyro.a, y: gyro.b, z: gyro.c });
	//const { x, y, z } = translateController.getAttribute("position");
	/*translateController.setAttribute("position", {
		x: x + dx1 + dx2,
		y: height,//-Pz
		z: z + dy1 + dy2
	});
	*/
};


socket.on("update_data", (data) =>
{
	console.log(data)
	updateView(data);
});




//setInterval(updateView, 100);
