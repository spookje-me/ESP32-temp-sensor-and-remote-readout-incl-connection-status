Temperature Sender and Receiver project, Dec 2024

For sender we use: 
Ideaspark®Esp32 16Mb Development Board 1.9in St7789 170X320 Tft Lcd-Scherm, Wifi + Bl Module, Ch340, Type-C Voor Arduino Micropython
together with temp sensor DS18b20.
Sensor is connected to pin 13 of ESP32 board. A 12V power separator is used and then a 12V to 5V DC-DC converter (used this setup to get rid of dirty power supply for blower fan in motor space.
Select “ESP32 Dev board” for this unit in Arduino.

The Ideaspark ESP32 unit sends the temp info to remote ESP32 readout unit with a round display (Esp32 Arduino Lvgl Wifi & Bluetooth Development Board 1.28 "240*240 Ips Slim Scherm 1.28 Inch Ips Lcd Tft Module).
This unit can get power from USB-C or via the small connector below the PCB. Side-switch needs to be pressed when this power option is used, this connection can also be used to charge a battery when USB-C is used as main power.
Select following in Arduino (very important, took me very long to figure out)
 Board: Use Adafruit QT PY ESP32-C3 board (in ESP32 section scroll down below until you find the board)
in Tools/Partition Scheme, select: Huge App (3MB No OTA / 1MB SPIFFS)
 

