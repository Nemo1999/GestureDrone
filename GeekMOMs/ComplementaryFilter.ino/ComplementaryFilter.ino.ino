
 
// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/
// ================================================================
// ===                    SETTING FOR ESP_NOW                   ===
// ================================================================
# include <espnow.h>
//# include <WiFi.h>
#include <ESP8266WiFi.h>
//ESP Board MAC Address:  A0:20:A6:1A:40:FB
uint8_t broadcastAddress[] = {0xA0, 0x20, 0xA6, 0x1A, 0x40, 0xFB};
#define BOARD_ID 2

typedef struct ESP_NOW_MESSAGE{
  int board_id;
  float gyroX;
  float gyroY;
  float gyroZ;
  float accX;
  float accY;
  float accZ;
}esp_now_message;

esp_now_message sender_message;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;


// ================================================================
// ===                    SETTING FOR MPU_6050                  ===
// ================================================================


// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "MPU6050.h"



// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;


// Use the following global variables and access functions to help store the overall
// rotation angle of the sensor
unsigned long last_read_time;
float         last_x_angle;  // These are the filtered angles
float         last_y_angle;
float         last_z_angle;  
float         last_gyro_x_angle;  // Store the gyro angles to compare drift
float         last_gyro_y_angle;
float         last_gyro_z_angle;

float    base_x_accel;
float    base_y_accel;
float    base_z_accel;

float    base_x_gyro;
float    base_y_gyro;
float    base_z_gyro;


void set_last_read_angle_data(unsigned long time, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro) {
  last_read_time = time;
  last_x_angle = x;
  last_y_angle = y;
  last_z_angle = z;
  last_gyro_x_angle = x_gyro;
  last_gyro_y_angle = y_gyro;
  last_gyro_z_angle = z_gyro;
}

inline unsigned long get_last_time() {return last_read_time;}
inline float get_last_x_angle() {return last_x_angle;}
inline float get_last_y_angle() {return last_y_angle;}
inline float get_last_z_angle() {return last_z_angle;}
inline float get_last_gyro_x_angle() {return last_gyro_x_angle;}
inline float get_last_gyro_y_angle() {return last_gyro_y_angle;}
inline float get_last_gyro_z_angle() {return last_gyro_z_angle;}



// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

void mpu_setup() {
    //accelgyro.start();
    last_read_time = millis();
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    //Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // calibration , TODO: set average value 
    
    accelgyro.CalibrateGyro(10);
    accelgyro.CalibrateAccel(10);
    accelgyro.PrintActiveOffsets();
    
    
  
    
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    
   
   set_last_read_angle_data(millis(),0,0,0,0,0,0);
}


// ================================================================
// ===       Read Gyro/Accel Data and Implement Filter          ===
// ================================================================

void mpu_loop() {
  unsigned long t_now = millis();
  
  // read raw accel/gyro measurements from device
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float FS_SEL = 131 ; //上面說的-32768 --- +32768 ，那麼這個數字到底代表了什麼呢？比如陀螺儀 32768 到底是指角速度達到多少度/秒 ？這個其實是根據MPU6050設定的量程來決定的，量程不一樣，32768代表的值就不一樣。按陀螺儀來說，MPU6050 有四個量程可選： ±250，±500，±1000，±2000 度/s 比方說，設定了是 ±250 , 那麼-32768  ---- +32768 就代表了 -250 ---- +250 。此時它的LSB(最低有效位) 是 131 LSB/(度/s)

  float gyro_x = (gx - base_x_gyro)/FS_SEL;
  float gyro_y = (gy - base_y_gyro)/FS_SEL;
  float gyro_z = (gz - base_z_gyro)/FS_SEL;

  // Get angle values from accelerometer
  float RADIANS_TO_DEGREES = 180/3.14159;
  float accel_angle_y = atan(-1*ax/sqrt(pow(ay,2) + pow(az,2)))*RADIANS_TO_DEGREES;
  float accel_angle_x = atan(ay/sqrt(pow(ax,2) + pow(az,2)))*RADIANS_TO_DEGREES;
  float accel_angle_z = atan(sqrt(pow(ay,2) + pow(ax,2)) / az) * RADIANS_TO_DEGREES;

  
  // Compute the (filtered) gyro angles
  float dt =(t_now - get_last_time())/1000.0;
  float gyro_angle_x = gyro_x*dt + get_last_x_angle();
  float gyro_angle_y = gyro_y*dt + get_last_y_angle();
  float gyro_angle_z = gyro_z*dt + get_last_z_angle();

  // Compute the drifting gyro angles
  float unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
  float unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
  float unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();
  
   // Apply the complementary filter to figure out the change in angle - choice of alpha is
  // estimated now.  Alpha depends on the sampling rate...
  float alpha = 0.96;
  float angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
  float angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
  float angle_z = gyro_angle_z;

  // Update the saved data with the latest values
  set_last_read_angle_data(t_now, angle_x, angle_y, angle_z, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);
  
  //Send the data to the serial port
  /*
  Serial.print(F("DEL:"));              //Delta T
  Serial.print(dt, DEC);
  Serial.print(F(" #raw_a:"));              //Accelerometer raw 
  Serial.printf("%2d",ax);
  Serial.print(F(","));
  Serial.printf("%2d",ay);
  Serial.print(F(","));
  Serial.printf("%2d",az);
  Serial.print(F(" #raw_g:"));              //Gyro  raw
  Serial.printf("%2d",gx);
  Serial.print(F(","));
  Serial.printf("%2d",gy);
  Serial.print(F(","));
  Serial.printf("%2d",gz);
  Serial.printf("\n");
  */
  /*
  Serial.print(F(" #ACC:"));              //Accelerometer angle
  Serial.print(accel_angle_x, 2);
  Serial.print(F(","));
  Serial.print(accel_angle_y, 2);
  Serial.print(F(","));
  Serial.print(accel_angle_z, 2);
  Serial.print(F("#GYR:"));
  Serial.print(unfiltered_gyro_angle_x, 2);        //Gyroscope angle
  Serial.print(F(","));
  Serial.print(unfiltered_gyro_angle_y, 2);
  Serial.print(F(","));
  Serial.print(unfiltered_gyro_angle_z, 2);
  Serial.print(F("#FIL:"));             //Filtered angle
  Serial.print(angle_x, 2);
  Serial.print(F(","));
  Serial.print(angle_y, 2);
  Serial.print(F(","));
  Serial.print(angle_z, 2);
  Serial.println(F(""));
  */  
  
  //Accelerometer doesn't give z-angle
  // these methods (and a few others) are also available
  //accelgyro.getAcceleration(&ax, &ay, &az);
  //accelgyro.getRotation(&gx, &gy, &gz);
  
  //Serial.printf("{\"gyro\":{\"x\":%3f,\"y\":%3f,\"z\":%3f}}\n",angle_x,angle_y,angle_z);
  //Serial.printf("{\"acc\":{\"x\":%d , \"y\":%d , \"z\":%d}}\n",ax,ay,az);
  sender_message.board_id = BOARD_ID;
  sender_message.gyroX = angle_x;
  sender_message.gyroY = angle_y;
  sender_message.gyroZ = angle_z;
  sender_message.accX = ax;
  sender_message.accY = ay;
  sender_message.accZ = az;
  esp_now_send(0, (uint8_t *) &sender_message, sizeof(sender_message));
  
  
  delay(50);
  
  
}


// ================================================================
// ===                  FUNCTIONS FOR ESP_NOW                   ===
// ================================================================


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}


void esp_setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  
}


// ================================================================
// ===                   main setup and loop                    ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void DataReady() {
    mpuInterrupt = true;
}

void loop(void)
{
  mpu_loop();
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println(F("\nOrientation Sensor OSC output")); Serial.println();
  esp_setup();
  mpu_setup();
}
