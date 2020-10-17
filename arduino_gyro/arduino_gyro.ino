/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

//set tty baudrate with 
//stty -F /dev/ttyUSB0 9600
//begin server with
//node ~/Documents/Arduino/p5seral.../startserver.js
// FROM  (https://github.com/p5-serial/p5.serialport)

#define rxPin 2
#define txPin 3

#define START_b 0x55
#define ACC_b 0x51
#define WCC_b 0x52
#define RPY_b 0x53

#define mtPin0 4
#define mtPin1 8
#define mtPin2 6
#define mtPin3 7


#include <SoftwareSerial.h>
SoftwareSerial mySerial(rxPin, txPin);
int cnt = 0;
signed int Acc[3];
signed int Wcc[3];
signed int Theta[3];
signed int Temp;
int* arr[3] = {Acc,Wcc,Theta};
int inline mergeBytes(byte h,byte l){return (int )l | ((int) h <<8);}


//Motor Patern
char pat[8][4]= {{0,1,1,1}
                ,{0,0,1,1}
                ,{1,0,1,1}
                ,{1,0,0,1}
                ,{1,1,0,1}
                ,{1,1,0,0}
                ,{1,1,1,0}
                ,{0,1,1,0}
                }
                ;
byte mtPins[4] = {mtPin0,mtPin1,mtPin2,mtPin3};
void inline setMotorPin(byte phase){
  for(byte i=0;i<4;i++)
    digitalWrite(mtPins[i] , (pat[phase][i])? HIGH : LOW);}

int mot_stepp = 0;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  for(byte i=0;i<4;i++)
  pinMode(mtPins[i] , OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  mySerial.begin(115200);
  
}
// the loop function runs over and over again forever
void loop() {
     
    if (mySerial.available()>0){
     if (mySerial.read()==START_b){
        while(true){
          unsigned short mode_num = mySerial.read() - ACC_b;
          if(mode_num<=2){
               for(byte i=0;i<3;i++){
                  while(!mySerial.available());
                  byte l = mySerial.read();
                  while(!mySerial.available());
                  byte h = mySerial.read();
                  arr[mode_num][i] = mergeBytes(h,l);
                }
                  while(!mySerial.available());
                  byte l = mySerial.read();
                  while(!mySerial.available());
                  byte h = mySerial.read();
                  Temp = mergeBytes( h , l);
                  break;
            }
          }   
      }
    }
  if(cnt%3000==0){
      mot_stepp++;
      byte mot_phase = mot_stepp%8;
      setMotorPin(mot_phase);
      //Serial.println(mot_phase);
      //for(char i=0;i<4;i++)
        //{Serial.print(pat[mot_phase][i],BIN);Serial.print(" , ");}
      //Serial.println();
    
  Serial.print(((float)Theta[0]/ 32768),4 );Serial.print(" , ");
  Serial.print(((float)Theta[1]/ 32768),4 );Serial.print(" , "); 
  Serial.print(((float)Theta[2]/ 32768),4 );Serial.print(" , "); 
  Serial.print(((float)Acc[0]/ 32768),4 );Serial.print(" , "); 
  Serial.print(((float)Acc[1]/ 32768),4 );Serial.print(" , "); 
  Serial.print(((float)Acc[2]/ 32768),4 );//Serial.print(" , ");
  Serial.print("\r\n"); 
  
  }
  //Serial.println(((float)Temp),4);}
  
  if (mySerial.overflow()) {
   Serial.println("SoftwareSerial overflow!!!!!!!!!!!!!!");
  }
  cnt++;
    
}
