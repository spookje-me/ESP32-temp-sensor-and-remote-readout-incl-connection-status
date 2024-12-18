/*
  ESP32 display temperature and send temp to ESP32 receiver
  Used for "ideaspark wifi esp32 wroom 32 30 pin layout"
  Board is ESP32 Dev Module
*/
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// REPLACE WITH YOUR RECEIVER MAC Address; example mac below is: 34:B7:DA:2D:5D:98
uint8_t broadcastAddress[] = {0x34, 0xB7, 0xDA, 0x2D, 0x5D, 0x98};

// TFT Display Pin Definitions
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define TFT_BL 32

// GPIO where the DS18B20 is connected to
const int oneWireBus = 14; 

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Initialize the TFT display with the appropriate pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Function to update TFT screen with Wi-Fi connection
void updateScreen() {
tft.fillScreen(ST77XX_BLACK);
}


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {

  float c;

} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Start the DS18B20 sensor
sensors.begin();

// Initialize the TFT display
tft.init(170, 320); // Initialize with 170x320 resolution
tft.setRotation(3); // Set the orientation to landscape
tft.fillScreen(ST77XX_BLACK); // Clear the screen
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(2);

// Initialize the backlight pin
pinMode(TFT_BL, OUTPUT);
digitalWrite(TFT_BL, HIGH); // HIGH =Turn on the backlight

tft.setTextColor(ST77XX_WHITE);
tft.println("");
tft.println("   White: Startup");
tft.println("");
tft.setTextColor(ST77XX_BLUE);
tft.println("   Blue:  30 < temp < 71");
tft.println("");
tft.setTextColor(ST77XX_GREEN);
tft.println("   Green: 71 < temp < 89");
tft.println("");
tft.setTextColor(ST77XX_RED);
tft.println("   Red:      > 89");
delay(5000);

 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
 
//  myData.c = temperatureC;

tft.setTextSize(4);
sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");

myData.c = temperatureC;

if (temperatureC > 0) {
  tft.setTextColor(ST77XX_WHITE);
  }

if ((temperatureC > 30) && (temperatureC <= 71)) {
  tft.setTextColor(ST77XX_BLUE);
  }

if ((temperatureC > 71) && (temperatureC <= 89)) {
  tft.setTextColor(ST77XX_GREEN);
  }

if (temperatureC > 89) {
  tft.setTextColor(ST77XX_RED);
  }

tft.fillScreen(ST77XX_BLACK);
tft.setTextSize(4);
tft.setCursor(40, 40);
tft.println("Motor temp");
tft.setCursor(75, 90);
tft.print(temperatureC,1);
tft.println("  C");
delay(2000);

  
 // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}