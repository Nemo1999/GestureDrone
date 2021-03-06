var gamepad = require("gamepad")
gamepad.init()

for (var i = 0, l = gamepad.numDevices(); i < l; i++) {
  console.log(i, gamepad.deviceAtIndex());
}

setInterval(gamepad.processEvents, 16);
// Scan for new gamepads as a slower rate
setInterval(gamepad.detectDevices, 500);

gamepad.on("move", function (id, axis, value) {
  console.log("move", {
    id: id,
    axis: axis,
    value: value,
  });
});

// Listen for button up events on all gamepads
gamepad.on("up", function (id, num) {
  console.log("up", {
    id: id,
    num: num,
  });
});

// Listen for button down events on all gamepads
gamepad.on("down", function (id, num) {
  console.log("down", {
    id: id,
    num: num,
  });
});