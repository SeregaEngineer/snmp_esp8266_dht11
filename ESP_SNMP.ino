#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SNMP_Agent.h>
#include <SNMPTrap.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define STATUS_LED 16

const char* ssid = "SSID";
const char* password = "PASSWORD";

WiFiUDP udp;
SNMPAgent snmp = SNMPAgent("public");  // Starts an SMMPAgent instance with the community string 'public'


unsigned long currentTime = 0; // текущее время работы
long previousMillis = 0;  // предыдущение сработка


int humidity;
int temperature;
bool temp = true;

void setup(){
    Serial.begin(115200);
    WiFi.hostname("sensor_server_room");
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    

    snmp.setUDP(&udp);
    snmp.begin();
    snmp.addIntegerHandler(".1.3.6.1.4.1.5.0", &temperature);
    snmp.addIntegerHandler(".1.3.6.1.4.1.6.0", &humidity);
    dht.begin();
    pinMode(STATUS_LED, OUTPUT);

    delay(500);
    Serial.end();

}

void loop(){
    snmp.loop(); // must be called as often as possible
    currentTime = millis();

    if (currentTime - previousMillis > 3000) // текущие - предыдущие
    {
        humidity = dht.readHumidity();
        temperature = dht.readTemperature();
        if (temperature < 0 || temperature > 50 )
        {
             temperature = -1;
        }
        if (humidity < 0 || humidity > 101 )
        {
             humidity = -1;
        }

        temp = !temp;
        digitalWrite(STATUS_LED, temp);

        //Serial.print("T: ");
        //Serial.println(temperature);
        //Serial.print("H: ");
        //Serial.println(humidity);
     previousMillis = currentTime;
   } 

}