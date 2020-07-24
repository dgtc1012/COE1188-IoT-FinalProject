
#include <itoa.h>
#include <Adafruit_TMP006.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define       WIFI_SSID         "RugbyHouse"
#define       WIFI_PWD          "Ba11sOut?"
#define       MQTT_CLIENT       "drg42_client"

// your network name also called SSID
char ssid[] = "RugbyHouse";

// your network password
char password[] = "Ba11sOut?";

// MQTTServer to use
char server[] = "test.mosquitto.org";

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

//TMP006
float tempReading = 0;
Adafruit_TMP006 tmp006;

#include "SPI.h"
#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;

//Initialization needed for the LCD screen
uint16_t x, y, x00, y00;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);

  Serial.println("Start WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  while(WiFi.localIP() == INADDR_NONE) 
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println("");

  printWifiStatus();
  
  //Print instructions to the LCD screen
  myScreen.clear();

  //Initialize the onboard lcd screen
  myScreen.begin();
  x00 = 0;
  y00 = 0;
  myScreen.clear();

  Serial.println("Welcome to the Temboo IoT LaunchPad Demo");
  wordWrap("Welcome to the Temboo IoT LaunchPad Demo");
  
  tmp006.begin(TMP006_CFG_8SAMPLE);  // Takes 8 averaged samples for measurement

  Serial.println("Setup complete.\n");
  //Print instructions to the LCD screen
  myScreen.clear();  
}

char *ftoa(char *a, float f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}

void loop() {
  // put your main code here, to run repeatedly: 

  //TMP006 TEMP SENSOR
  float temp = tmp006.readObjTempC();
  char* tempText;
  char ii;
  tempText = (char*) malloc(10);
  for (ii=0;ii<10;ii++)
    tempText[ii] = 0;
  tempText =  ftoa(tempText, temp, 4);

  //String output = "New Temp Reading: " + String(tempText);
  //Serial.println(output);
  //wordWrap(output);

  if(client.connect("drg42_client")) 
  {
    Serial.println("MQTT Connection success");
    
    if(client.subscribe("node-red_out")) 
       Serial.println("Subscription successfull");
  }

  client.publish("node-red_in", tempText);
  Serial.println("Publishing successful!");    
        
  // Check if any message were received
  // on the topic we subsrcived to
  client.poll();  

  
  
  delay(1000);
}

//Used to wordWrap a string on the LCD, maxChars is the number of characters to write per line
void wordWrap(String input){  
  String line = "";
  int s = 0;
  int maxChars = 20;
  //start coordinates
  x = 5;
  y = 40;
  
  boolean lastLine = false;
  boolean wrap;
  myScreen.clear();
  while(lastLine != true){
    wrap = false;
    line = "";
    for(int i = s;i<s+maxChars;i++){
      if (input[i] == '\0'){
        lastLine = true;
      }
      else if (input[i] == ' '){
        wrap = true;
      }
    }
    if(lastLine == true){
      while(input[s] != '\0'){
        line += input[s];
        s++;
      }
    }
    else if (wrap == true){
      for(int i = s;i<input.lastIndexOf(" ",s+maxChars);i++){
        line += input[i];
      }
      s = input.lastIndexOf(" ",s+maxChars)+1;
    }
    else{
      for (int i = s; i<s+maxChars;i++){
        line += input[i];
      }
      line += '-';
      s += maxChars;
    }
    myScreen.gText(x,y,line);
    y += 10;
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Received message for topic ");
  Serial.print(topic);
  Serial.print(" with length ");
  Serial.println(length);
  Serial.println("Message:");
  Serial.write(payload, length);
  Serial.println();
  
  if(payload[0] == 49){
    if(length == 4){
      Serial.println("Found an outlier and pple are unhappy tweeting about #trump");
      wordWrap("Found an outlier and pple are unhappy tweeting about #trump");
    }
    else if(payload[2] == 49){
      Serial.println("Found an outlier and pple are happy tweeting about #trump");
      wordWrap("Found an outlier and pple are happy tweeting about #trump");
    }
    else{
      Serial.println("Found an outlier and pple are neutral tweeting about #trump");
      wordWrap("Found an outlier and pple are neutral tweeting about #trump");
    }
  }
  else{
      if(length == 4){
      Serial.println("No outlier and pple are unhappy tweeting about #trump");
      wordWrap("No outlier and pple are unhappy tweeting about #trump");
    }
    else if(payload[2] == 49){
      Serial.println("No outlier and pple are happy tweeting about #trump");
      wordWrap("No outlier and pple are happy tweeting about #trump");
    }
    else{
      Serial.println("No outlier and pple are neutral tweeting about #trump");
      wordWrap("No outlier and pple are neutral tweeting about #trump");
    }
  }
}

