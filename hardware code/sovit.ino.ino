#define SS_PIN 4  //D2
#define RST_PIN 5 //D1 rfid sensor
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <MFRC522.h>

#include <SPI.h>
const char *ssid = "Galaxy-M20"; // Enter your WiFi Name
const char *pass = "ac312124"; // Enter your WiFi Password

String msg;
char msg1[20];
WiFiClient client;

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "choudharyas"
#define MQTT_PASS "988c4e045ef64c1b9bc8b5bb7ef5f2d9"

void send_event(const char *event);
const char *host = "maker.ifttt.com";
const char *privateKey = "hUAAAz0AVvc6-NW1UmqWXXv6VQWmpiGFxx3sV5rnaM9";


FPS_GT511C3 fps(D6, D5);
#define relay D1
#define buzzer D2

Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
//Set up the feed you'r publishing to
Adafruit_MQTT_Publish Fingerprint = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/Fingerprint");
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;

void setup()
{
Serial.begin(9600); //set up Arduino's hardware serial UART
delay(100);
fps.Open(); //send serial command to initialize fps
fps.SetLED(true); //turn on LED so fps can see fingerprint
pinMode(relay, OUTPUT);
digitalWrite(relay, LOW);

Serial.println("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, pass);
WiFi.begin(ssid, password);
  
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print("."); // print ... till not connected
}
Serial.println("");
Serial.println("WiFi connected"); 
printWiFiStatus(); //Print the new IP to Serial
}

void loop()
{
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
 
}
MQTT_connect();
if (fps.IsPressFinger())
{
fps.CaptureFinger(false);
int id = fps.Identify1_N();

if (id ==7) //<- change id value depending model you are using
{
Serial.print("entered");
digitalWrite(relay, HIGH);
delay(6000);
digitalWrite(relay, LOW);
msg = "Entered";
msg.toCharArray(msg1, 10);
if (! Fingerprint.publish(msg1))
{
delay(500);
}
}
else if (id ==8)
{
Serial.print("Entered");
digitalWrite(relay, HIGH);
delay(6000);
digitalWrite(relay, LOW);
msg = "2 Entered";
msg.toCharArray(msg1, 10);
if (! Fingerprint.publish(msg1))
{
delay(500);
}
}

else if (id ==9)
{
Serial.print("Entered");
digitalWrite(relay, HIGH);
delay(6000);
digitalWrite(relay, LOW);
msg = "3Entered";
msg.toCharArray(msg1, 10);
if (! Fingerprint.publish(msg1))
{
delay(500);
}
}

WiFiClient client = server.available();
  
else if (client) {
    
    Serial.println("Client connected.");
    
    while (client.connected()) {  
      
      if (client.available()) {
        
        char command = client.read(); 
        
        if (command == 'H') {
          
          digitalWrite(relay, HIGH);
          Serial.println("device entered");
        }

    

 else if (content.substring(1) == "8E 39 32 50") //change UID of the card that you want to give access
  {
    Serial.println(" Access Granted ");
    Serial.println(" Welcome");
    delay(1000);
    Serial.println();
    statuss = 1;
  }
else
{
send_event("login_event");
Serial.println("Finger not found");
for(int i = 7; i > 0; i--){
Serial.print("Wrong");
}

}
}
else
{
Serial.println("Please press finger");
}
delay(100);


}

void send_event(const char *event)
{
Serial.print("Connecting to ");
Serial.println(host);
WiFiClient client;
const int httpPort = 80;
if (!client.connect(host, httpPort)) {
Serial.println("Connection failed");
return;
}

// We now create a URI for the request
String url = "/trigger/";
url += event;
url += "/with/key/";
url += privateKey;

Serial.print("Requesting URL: ");
Serial.println(url);

// This will send the request to the server
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"Connection: close\r\n\r\n");
while(client.connected())
{
if(client.available())
{
String line = client.readStringUntil('\r');
Serial.print(line);
} else {
// No data yet, wait a bit
delay(50);
};
}

Serial.println();
Serial.println("closing connection");
client.stop();
}

void MQTT_connect()
{
int8_t ret;

// Stop if already connected.
if (mqtt.connected())
{
return;
}

uint8_t retries = 3;
while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
{

mqtt.disconnect();
delay(5000); // wait 5 seconds
retries--;
if (retries == 0)
{
// basically die and wait for WDT to reset me
while (1);
}
}
}
