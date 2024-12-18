/*
* Use Adafruit QT PY ESP32-C3 board (in ESP32 section scroll down below until you find the board)
* in Tools/Partition Scheme, select: Huge App (3MB No OTA / 1MB SPIFFS)
*/

#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0xe0, 0xe2, 0xe6, 0x2f, 0xfc, 0x5c};


typedef struct test_struct {
  int x;
} test_struct;

test_struct test;

esp_now_peer_info_t peerInfo;


// Setup TFT screen

Arduino_DataBus *bus = new Arduino_ESP32SPI(2 /* DC */, 10 /* CS */, 6 /* SCK */, 7 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, -1 /* RST */, 0 /* rotation */, true /* IPS */);

#define GFX_BL DF_GFX_BL  // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#define TFT_BL 3


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {

// c = temperatureC from sending module (=myData.c)
    float c;
      
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
//  Serial.println(myData.c);
// Serial.println();
}
 


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
// Initialize the TFT display
gfx->begin();
gfx->fillScreen(BLACK);

// Initialize the backlight pin
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif

 // Initial message to display

gfx->setTextSize(2);

gfx->setCursor(40, 80);
gfx->setTextColor(WHITE);
gfx->println("White: Startup");

gfx->setCursor(40, 105);
gfx->setTextColor(BLUE);
gfx->println("Blue:  30<T<71");

gfx->setCursor(40, 130);
gfx->setTextColor(GREEN);
gfx->println("Green: 71<T<89");

gfx->setCursor(42, 155);
gfx->setTextColor(RED);
gfx->println("Red:   T > 89");

delay(2000);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);


if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
   memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];  
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Connected" : "Not Connected");
  String ConnectionStatus = (status == ESP_NOW_SEND_SUCCESS ? "<^>" : " . ");

// Serial.println(ConnectionStatus);
  gfx->setTextSize(2);
  gfx->setCursor(105, 210);
  gfx->setTextColor(YELLOW);
  gfx->println(ConnectionStatus);

  delay(2000);

  //gfx->setTextColor(WHITE);

}


void loop() {

// REPLACE WITH YOUR RECEIVER MAC Address; example mac below is: 34:B7:DA:2D:5D:98
//uint8_t broadcastAddress[] = {0xe0, 0xe2, 0xe6, 0x2f, 0xfc, 0x5c};

test.x = random(0,20);
  esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));


  delay(2000);

  if ((myData.c == 0)) {
  Serial.println("Connecting");
  gfx->setTextColor(RED);
  gfx->fillScreen(BLACK);
  gfx->setTextSize(3);
  gfx->setCursor(33, 110);
  gfx->println("Connecting");
  delay(1000);
} 
else {
  Serial.print(myData.c,1);
  Serial.println("ºC");

if (myData.c > -150) {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  }

if ((myData.c > 30) && (myData.c <= 71)) {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(BLUE);
  }

if ((myData.c > 71) && (myData.c <= 89)) {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(GREEN);
  }

if (myData.c > 89) {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(RED);
  }

//delay(1000);

//gfx->flush();
gfx->fillScreen(BLACK);
gfx->setTextSize(2);
gfx->setCursor(62, 54);
gfx->println("Motor temp");
gfx->setTextSize(5);
gfx->setCursor(60, 115);
gfx->print(myData.c,1);
gfx->setTextSize(2);
gfx->println(" o");

delay(500);

}
}