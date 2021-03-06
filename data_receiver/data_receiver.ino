#include <ESP8266WiFi.h>
#include <espnow.h>

// Structure example to receive data
// Must match the sender structure
typedef struct ESP_NOW_MESSAGE{
  int board_id;
  float gyroX;
  float gyroY;
  float gyroZ;
  float accX;
  float accY;
  float accZ;
}esp_now_message;

volatile bool received[3] = {false,false,false};

// Create a struct_message called myData
esp_now_message myData;

// Create a structure to hold the readings from each board
esp_now_message board1;
esp_now_message board2;
esp_now_message board3;

// Create an array with all the structures
esp_now_message boardsStruct[3] = {board1, board2,board3};

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  memcpy(&myData, incomingData, sizeof(myData));
  
  // Update the structures with the new incoming data
  boardsStruct[myData.board_id-1].gyroX = myData.gyroX;
  boardsStruct[myData.board_id-1].gyroY = myData.gyroY;
  boardsStruct[myData.board_id-1].gyroZ = myData.gyroZ;
  boardsStruct[myData.board_id-1].accX = myData.accX;
  boardsStruct[myData.board_id-1].accY = myData.accY;
  boardsStruct[myData.board_id-1].accZ = myData.accZ;
  received[myData.board_id-1] = true;
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("setup");
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  
  for(int i=0;i<3;i++){
    boardsStruct[i].gyroX = 0;
    boardsStruct[i].gyroY = 0;
    boardsStruct[i].gyroZ = 0;
    boardsStruct[i].accX = 0;
    boardsStruct[i].accY = 0;
    boardsStruct[i].accZ = 0;
  }

}

void loop(){
  // Access the variables for each board
  if(/*received[0] &&*/ received[1] /*&& received[2]*/)
  {
    //Serial.printf("{\"gyro%d\":{\"x\":%3f,\"y\":%3f,\"z\":%3f},\"acc%d\":{\"x\":%3f,\"y\":%3f,\"z\":%3f}}\n",myData.board_id,boardsStruct[myData.board_id-1].gyroX,boardsStruct[myData.board_id-1].gyroY,boardsStruct[myData.board_id-1].gyroZ,myData.board_id,boardsStruct[myData.board_id-1].accX, boardsStruct[myData.board_id-1].accY, boardsStruct[myData.board_id-1].accZ);
    //Serial.printf("%d,%3f,%3f,%3f,%3f,%3f,%3f\n",myData.board_id,boardsStruct[myData.board_id-1].gyroX,boardsStruct[myData.board_id-1].gyroY,boardsStruct[myData.board_id-1].gyroZ,boardsStruct[myData.board_id-1].accX, boardsStruct[myData.board_id-1].accY, boardsStruct[myData.board_id-1].accZ);
    for(int i=0;i<3;i++){
      Serial.printf("%d,%3f,%3f,%3f,%3f,%3f,%3f\n",i,boardsStruct[i].gyroX,boardsStruct[i].gyroY,boardsStruct[i].gyroZ,boardsStruct[i].accX, boardsStruct[i].accY, boardsStruct[i].accZ);
      received[i] = false;  
    }
    
    
    
    //Serial.print("Packet received from: ");
    //Serial.printf("Board ID %d:\n", boardsStruct[myData.board_id-1].board_id);
    //Serial.printf("x value: %d \n", boardsStruct[myData.board_id-1].x);
    //Serial.printf("y value: %d \n", boardsStruct[myData.board_id-1].y);
    //Serial.printf("z value: %d \n", boardsStruct[myData.board_id-1].z);
    //Serial.println()
  
    
  }
  
  
  //int board2X = boardsStruct[1].x;
  //int board2Y = boardsStruct[1].y;
  
}
