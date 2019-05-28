# NodeMUC_v1_7_ESP8266_7Segment_webserver
NodeMCU v 1.7 w/ESP 8266 and 7-segment display (with HT16K33 controller) and webserver
This code is based on the following projects

+Youtube subscription counter: https://www.instructables.com/id/YouTube-Subscriber-Counter-With-ESP8266-V2/
+7 Segment LED display control: https://www.adafruit.com/product/1269
+ESP8266 Webserver control (with web interface): https://randomnerdtutorials.com/esp8266-web-server-with-arduino-ide/

Hardware wiring
-For the 7-segment display with HT16K33 controller
+"+" to vin
+"-" to GND
+S(C)L to D1
+S(D)A to D2

I2C address settings (0x70) for the 7-segment display module

*Before running the code make sure you have updated the wifi information, youtube API key (if you want to use the counter)
and the channel ID

v1.0
Upon connection to the server and getting an IP address, you can control the LED to display "GO"/"STOP" by
visiting the IP address of the NodeMCU (you can get it via the Serial monitor).
