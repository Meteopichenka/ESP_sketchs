#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>

#define DHTTYPE DHT11   // DHT 11
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
#define dht_dpin0 0
#define dht_dpin1 2


Adafruit_BMP280 bme; // I2C


DHT dht1(dht_dpin0, DHTTYPE); 
DHT dht2(dht_dpin1, DHTTYPE); 

const char *ssid = "Boston Dynamics";
const char *password = "london18";
const char *host = "s3.skop.in.ua";
const int port = 9090;

void setup(void)
{ 
  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);
  
  dht1.begin();
  dht2.begin();
  
  if (!bme.begin()) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  
  Serial.println("Start\n\n");
  delay(700);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // This is your NodeMCU IP address. Could be handy for other projects

  digitalWrite(LED_BUILTIN, LOW);

}

void loop() {
    
    float h1 = dht1.readHumidity();
    float t1 = dht1.readTemperature();         
    //Serial.print("Current humidity = ");
    Serial.print(h1);
    Serial.print("%  ");
    //Serial.print("temperature = ");
    Serial.print(t1); 
    Serial.print("°C  ");

    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();         
    //Serial.print("Current humidity = ");
    Serial.print(h2);
    Serial.print("%  ");
    //Serial.print("temperature = ");
    Serial.print(t2); 
    Serial.print("°C  ");
    
    //Serial.print("Temperature=");
    float t3 = bme.readTemperature();
    float p = bme.readPressure();
    Serial.print(t3);
    Serial.print("°C  ");
    
    //Serial.print(" Presure=");
    Serial.print(p);
    Serial.print("Pa  ");
    
    digitalWrite(LED_BUILTIN, HIGH);
    
    send(t1,h1,t2,h2,t3,p);

    digitalWrite(LED_BUILTIN, LOW);
    delay(10000);

}

void send(float t1, float h1, float t2, float h2,float t3,float p){
  String pubString;
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  pubString = "Value: " + String(t1) + " " + String(h1) + " " + String(t2) + " " + String(h2) + " " + String(t3) + " " + String(p);
  String pubStringLength = String(pubString.length(), DEC);
  // We now create a URI for the request
  Serial.print("Requesting POST: ");
  client.print("Host:");
  client.print(host);
  client.println();
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(pubStringLength);
  client.println();
  client.print(pubString);
  client.println();
  delay(500); // Can be changed
  
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection");
  
}


