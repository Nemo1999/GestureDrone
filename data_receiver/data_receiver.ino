#include <ESP8266WiFi.h>
#include <espnow.h>

// Structure example to receive data
// Must match the sender structure
typedef struct ESP_NOW_MESSAGE{
  int board_id;
  float x;
  float y;
  float z;
}esp_now_message;

volatile bool received = false;

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
  boardsStruct[myData.board_id-1].x = myData.x;
  boardsStruct[myData.board_id-1].y = myData.y;
  boardsStruct[myData.board_id-1].z = myData.z;
  received = true;
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
  
}

void loop(){
  // Access the variables for each board
  if(received)
  {
    Serial.printf("{\"gyro%d\":{\"x\":%3f,\"y\":%3f,\"z\":%3f}}\n",myData.board_id,boardsStruct[myData.board_id-1].x,boardsStruct[myData.board_id-1].y,boardsStruct[myData.board_id-1].z);
    //Serial.print("Packet received from: ");
    //Serial.printf("Board ID %d:\n", boardsStruct[myData.board_id-1].board_id);
    //Serial.printf("x value: %d \n", boardsStruct[myData.board_id-1].x);
    //Serial.printf("y value: %d \n", boardsStruct[myData.board_id-1].y);
    //Serial.printf("z value: %d \n", boardsStruct[myData.board_id-1].z);
    //Serial.println();
    received = false;  
  }
  
  int board1X = boardsStruct[0].x;
  int board1Y = boardsStruct[0].y;
  //int board2X = boardsStruct[1].x;
  //int board2Y = boardsStruct[1].y;
  
}
