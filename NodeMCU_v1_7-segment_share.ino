// NodeMCU_v1_7- 7 segment with webserver
// by Hamlet Lin
// based on YouturbSubscriberCounter by Becky Stern 2018 
// *The Youtube library is disabled in this example

// Displays your current subscriber count on a seven-segment display
// This version uses two four digit displays to support more than 10k subs

//   based on library sample code by:
//   Giacarlo Bacchio (Gianbacchio on github)
//   Brian Lough (witnessmenow on github)
//   Adafruit (adafruit on github)

// requires the following libraries, search in Library Manager or download from github:
#include <Wire.h>                  // installed by default
#include <Adafruit_GFX.h>          // https://github.com/adafruit/Adafruit-GFX-Library
#include "Adafruit_LEDBackpack.h"  // https://github.com/adafruit/Adafruit_LED_Backpack
#include <YoutubeApi.h>            // https://github.com/witnessmenow/arduino-youtube-api
#include <ArduinoJson.h>           // https://github.com/bblanchon/ArduinoJson

// these libraries are included with ESP8266 support
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

char ssid[] = "";       // your network SSID (name)
char password[] = "";  // your network key

//------- enter your API key and channel ID here! ------
#define API_KEY ""  // your google apps API Token
#define CHANNEL_ID "UCu7_D0o48KbfhpEohoP7YSQ" // makes up the url of channel


// label the displays with their i2c addresses
struct {
  uint8_t           addr;         // I2C address
  Adafruit_7segment seg7;         // 7segment object
} disp[] = {
  { 0x71, Adafruit_7segment() },  // High digits
  { 0x70, Adafruit_7segment() }   // Low digits
};
String ipA[4] ={"0", "0", "0", "0"};

int subscriberCount; // create a variable to store the subscriber count

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs = 1000; //mean time between api requests
unsigned long api_lasttime;   //last time api request has been done

// Set web server port number to 80
//web server reference: https://randomnerdtutorials.com/esp8266-web-server-with-arduino-ide/
//7-segment library reference: https://forum.arduino.cc/index.php?topic=480922.0
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;

void setup() {

  Serial.begin(9600);
  
  for(uint8_t i=0; i<2; i++) {       // Initialize displays
    disp[i].seg7.begin(disp[i].addr);
    disp[i].seg7.clear();
    disp[i].seg7.writeDisplay();
  }
  
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  server.begin();
}

int counter =0;
void loop() {
  /*
   //code segment for displaying ip address on the 7-segment
  if (millis()%1000 <1)
  {
    
   IPAddress ip = WiFi.localIP();     
   int ipPrint = ip[counter];
   int TenthOf = pow(10, (String(ipPrint).length()-1));
   
   disp[1].seg7.print(ipPrint,DEC);  
   disp[1].seg7.writeDigitNum(4,  ipPrint%10, true); 
  if (counter == 0)
   {
    disp[1].seg7.drawColon(true);
    disp[1].seg7.writeColon();
   }else{
       disp[1].seg7.drawColon(false);
    disp[1].seg7.writeColon();
   }

   disp[1].seg7.writeDisplay();
   counter++; 
   if (counter>3)
      {
        counter = 0;
      } 
  }
 */
  //http server
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              //digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              //digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              //digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              //digitalWrite(output4, LOW);
            }
            

            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }    
  
  //control the digit of the 7-segment with client info
    if (output5State == "off")
    {
      //Serial.println("output5State on"); //stop
      //disp[1].seg7.writeDigitNum(1, 5);
      disp[1].seg7.writeDigitRaw(0,109);
      disp[1].seg7.writeDigitRaw(1,7);
      disp[1].seg7.writeDigitNum(3,0);
      disp[1].seg7.writeDigitRaw(4,115);
      disp[1].seg7.writeDisplay();
    }else if (output5State == "on"){
      //Serial.println("output5State off"); //go
      //disp[1].seg7.writeDigitNum(1, 0);
      //disp[1].seg7.writeDisplay();
      disp[1].seg7.writeDigitRaw(0,0x00);
      disp[1].seg7.writeDigitRaw(1, 61);
      disp[1].seg7.writeDigitNum(3,0);
      disp[1].seg7.writeDigitRaw(4,0x00);
      disp[1].seg7.writeDisplay();
    }
   /*
    *code segment for the other button of the webserver
  if (output4State == "on")
      {
        //Serial.println("output5State on");
        disp[1].seg7.writeDigitNum(4, 4);
        disp[1].seg7.writeDisplay();
      }else if (output5State == "off"){
        //Serial.println("output5State off");
        disp[1].seg7.writeDigitNum(4, 0);
        disp[1].seg7.writeDisplay();
      }  
      */ 

/*          
 *    Original youtube subscriber count code segment
if (millis() > api_lasttime + api_mtbs)  {
    if(api.getChannelStatistics(CHANNEL_ID))
    {
      Serial.println("---------Stats---------");
      Serial.print("Subscriber Count: ");
      Serial.println(api.channelStats.subscriberCount);
      Serial.print("View Count: ");
      Serial.println(api.channelStats.viewCount);
      Serial.print("Comment Count: ");
      Serial.println(api.channelStats.commentCount);
      Serial.print("Video Count: ");
      Serial.println(api.channelStats.videoCount);
      // Probably not needed :)
      //Serial.print("hiddenSubscriberCount: ");
      //Serial.println(api.channelStats.hiddenSubscriberCount);
      Serial.println("------------------------");
      
      subscriberCount = api.channelStats.subscriberCount;
      
      uint16_t hi = subscriberCount / 10000, // Value on left (high digits) display
               lo = subscriberCount % 10000; // Value on right (low digits) display
      disp[0].seg7.print(hi, DEC);   // Write values to each display...
      disp[1].seg7.print(lo, DEC);

      // print() does not zero-pad the displays; this may produce a gap
      // between the high and low sections. Here we add zeros where needed...
      if(hi) {
        if(lo < 1000) {
          disp[1].seg7.writeDigitNum(0, 0);
          if(lo < 100) {
            disp[1].seg7.writeDigitNum(1, 0);
            if(lo < 10) {
              disp[1].seg7.writeDigitNum(3, 0);
            }
          }
        }
       } else {
         disp[0].seg7.clear(); // Clear 'hi' display
        }
       disp[0].seg7.writeDisplay(); // Push data to displays
       disp[1].seg7.writeDisplay();
      }
      api_lasttime = millis();
    }
 */
}

char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}
