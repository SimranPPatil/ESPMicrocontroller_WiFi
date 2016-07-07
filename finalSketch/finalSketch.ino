#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//for LED status
#include <Ticker.h>
Ticker ticker;
int val = 0;
int inputPin = 14;
void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void userInterrupt(){
    val = 1;
}
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("val is");
  Serial.println(val);
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);
  //Serial.println("input is ");
  //Serial.println(val);
  attachInterrupt(inputPin, userInterrupt, RISING);
  //Serial.println("after interrupt, input is ");
  //Serial.println(val);

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
  WiFiManager wifiManager;

  if(val == 1){
   Serial.println("post interrupt in the loop");
   WiFi.disconnect(true);
   delay(1000);
   val = 0;
   Serial.println("wifi ssid now ");
   Serial.println(WiFi.SSID());
   if(WiFi.SSID() == "" || WiFi.SSID() == NULL){
      ticker.attach(0.2, tick);
   }
   wifiManager.autoConnect();
  }
 
  //put your main code here, to run repeatedly:
  Serial.println("wifi status is ");
  Serial.println(WiFi.status());
  if(WiFi.status()!=WL_CONNECTED){
    Serial.println("failed to connect, retrying ");
    ticker.attach(0.6, tick);
  }
  else{
    Serial.println("Connected here");
    ticker.detach();
    digitalWrite(BUILTIN_LED, LOW);
  }
  wifiManager.checkConnection();
  delay(5000);
}
