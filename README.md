# GestureDrone

- P5 folder can be serve with static file server and viewed in browser
  for example :

```bash
	npm install --global http-server
	cd .../GestureDrone/P5 
	http-server .
```
- One can also upload the files(except "p5.js") to p5.js [online editor](https://editor.p5js.org/)

- Currently send gyroscope information from computer serial port and then to the p5 webpage using
https://github.com/p5-serial/p5.serialport
  (This should be changed by wifi capability of NodeMCU)

- Currently arduino code is for a special gyro , not the typical MPU-6050
  (This should be changed too)
