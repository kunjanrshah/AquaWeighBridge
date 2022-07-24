#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "kunjanrshah@gmail.com";
const char* mqttPassword = "kunjan@123";
int buttonState = 0;
int counter=0; 
const int buttonPin = 2; 
const int ledConnected = 4;
const int ledSend = 5;
const char* WIFI_NAME="SuperbScale1";  //change
const char* MQTT_TOPIC="kunjan/superb1"; //change
const char* MQTT_CLIENT="SuperbScale";

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;

void setup() {
  
  Serial.begin(115200);
  pinMode(ledSend, OUTPUT);
  pinMode(ledConnected, OUTPUT);
  buttonState = digitalRead(buttonPin);
  wifiManager.autoConnect(WIFI_NAME);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.println("Connecting to WiFi..");
  }
  //Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
   // Serial.println("Connecting to MQTT...");
    if (client.connect(MQTT_CLIENT, mqttUser, mqttPassword )) {
    //  Serial.println("connected");  
    } else {     
   //   Serial.print("failed with state ");
    //  Serial.print(client.state());
      delay(2000);
    }
  }
 
  client.publish(MQTT_TOPIC, "0.000");
  client.subscribe(MQTT_TOPIC);
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 
 // Serial.print("Message arrived in topic: ");
 // Serial.println(topic);
 // Serial.print("Message:");
  String str="";
  for (int i = 0; i < length; i++) {
    str=str+(char)payload[i];
  }
  Serial.println(str);
  payload=0;
  str="";
}
 
void loop() {

  if(Serial.available()>0)    //Checks is there any data in buffer 
  {   
    String str1=Serial.readString(); 
    client.publish(MQTT_TOPIC, str1.c_str()); 
    digitalWrite(ledSend, HIGH);    
  }
   delay(1000);
   digitalWrite(ledSend, LOW);

   if(client.connected()){
      delay(2000);
      digitalWrite(ledConnected, HIGH);
      counter=0;
    }else{ 
         while (!client.connected()) {
           if (client.connect(MQTT_CLIENT, mqttUser, mqttPassword )) {
            //  Serial.println("connected");  
            counter=0;
          } else {
            counter++;
      
            digitalWrite(ledConnected, LOW);
//            Serial.print("Counter : ");
//            Serial.println(counter);
            delay(3000);
            if(counter>100){
                counter=0;
//              Serial.print("going to reset");
//              Serial.println(); 
              exit(0);
            }
          }
        }
    }
   
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
        wifiManager.resetSettings();
        exit(0);
  }
  client.loop();
}
