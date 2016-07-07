#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>            //needed for library
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Ticker.h>               //for LED status

Ticker ticker;
int val = 0;  //flag indicating button pressed (interrupt)
int inputPin = 14; //connected to the input

void tick(){
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void userInterrupt(){
  val = 1;  //flag indicating pin HIGH
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager){
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick); //indicates Access Point mode
}

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("val is");
  Serial.println(val);  //debugging - current input val at the start
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick); //connecting...
  attachInterrupt(inputPin, userInterrupt, RISING);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings(); 
  //wifiManager.myReset();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit ti meout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);
}

void loop() {
  WiFiManager wifiManager; //declaring an object to access the Wifimanager functions

  if(val == 1){ //implies that the button was pressed, should clear ssid and pw and become an AP
     Serial.println("post interrupt in the loop"); //debug
     WiFi.disconnect(true); //this clears ssid/password
     delay(1000); //required
     val = 0; //undo the flag so that doesn't enter this condition again
     Serial.println("wifi ssid now ");
     Serial.println(WiFi.SSID()); //debug , should be empty
     if(WiFi.SSID() == "" || WiFi.SSID() == NULL){
        ticker.attach(0.2, tick); // to indicate access point set up
     }
     wifiManager.autoConnect();// getting back into the AP setup :)
   }
 
  Serial.println("wifi status is ");
  Serial.println(WiFi.status()); //debug
  if(WiFi.status()!=WL_CONNECTED){
    Serial.println("failed to connect, retrying ");
    ticker.attach(0.6, tick); //looking for network
  }
  else{
    Serial.println("Connected here");
    ticker.detach();
    digitalWrite(BUILTIN_LED, LOW); //implies no ticking - connected
  }
  wifiManager.checkConnection(); //keeps checking for the network in each interval to look for disconnectivity, if it occurs
  delay(5000); //neat :)
}
