/*
 * Copyright (c) 2024/2025 Tobias Guggenberger - software@guggenberger.me
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Ethernet.h>
#include <PubSubClient.h> // mqtt
#include "PCF8574.h" // Port Expander
#include <SPI.h> // Seriell
#include <Wire.h>

/************************************************************************** Kabelcode
gn        SDA
gn/ 1sw   SDC
gr        GND
rt        +5V
*/

/************************************************************************** Pinbelegung Arduino UNO
A4      SDA -> gn
A5      SCL -> gn/1sw
*/

//************************************************************************** PCF8574 Init
PCF8574 pcf8574(0x20);

//************************************************************************** LAN Network definieren 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01 };
IPAddress ip(10, 110, 0, 5); //comment this line if you are using DHCP

//IPAddress subnet(255, 255, 0, 0); // Subnet Mask

IPAddress mqtt_server(10, 110, 255, 250);  // IP-Adresse des MQTT Brokers im lokalen Netzwerk

EthernetClient ethClient;
PubSubClient client(ethClient);

//************************************************************************** mqtt Server


//************************************************************************** Funktionsprototypen
void loop                       ();
void setup                      ();
void reconnect                  ();
void callback(char* topic, byte* payload, unsigned int length);


//************************************************************************** Intervalle
/* Beispiel
unsigned long previousMillis_BEISPIEL = 0; // 
unsigned long interval_BEISPIEL = 800; 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ BEISPIEL
  if (millis() - previousMillis_BEISPIEL > interval_BEISPIEL) {
      previousMillis_BEISPIEL = millis(); 
      FUNKTION();
    }  

*/


//************************************************************************** SETUP
void setup() {
  Serial.begin(115200);

// ------------------------------------------------------------------------- Ethernet starten
  Ethernet.begin(mac, ip);
  // Pause Netzwerk Antwort
  delay(1500);  

///////////////////////////////////////////////////////////////////////////  MQTT Broker init
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

/////////////////////////////////////////////////////////////////////////// Konfig Portexpander
  pcf8574.pinMode(P0, OUTPUT);
  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.pinMode(P2, OUTPUT);
  pcf8574.pinMode(P3, OUTPUT);
  pcf8574.pinMode(P4, OUTPUT);
  pcf8574.pinMode(P5, OUTPUT);
  pcf8574.pinMode(P6, OUTPUT);
  pcf8574.pinMode(P7, OUTPUT);        
  pcf8574.begin();

/////////////////////////////////////////////////////////////////////////// Portexpander Relais definieren
pcf8574.digitalWrite(P0, !LOW);
pcf8574.digitalWrite(P1, !LOW);
pcf8574.digitalWrite(P2, !LOW);
pcf8574.digitalWrite(P3, !LOW);
pcf8574.digitalWrite(P4, !LOW);
pcf8574.digitalWrite(P5, !LOW);
pcf8574.digitalWrite(P6, !LOW);
pcf8574.digitalWrite(P7, !LOW);

}

//************************************************************************** mqtt - reconnect
void reconnect() {
  while (!client.connected()) {
    Serial.print("Verbindung zum MQTT-Server aufbauen...");
    if (client.connect("WerktorClient", "hitesh", "RO9UZ7wANCXzmy")) {
      Serial.println("verbunden");
      client.subscribe("Werktor/K0");
    } else {
      Serial.print("Fehler, rc=");
      Serial.print(client.state());
      Serial.println(" erneut versuchen in 5 Sekunden");
      delay(5000);
    }
  }
}

//************************************************************************** mqtt - callback
void callback(char* topic, byte* payload, unsigned int length) {

 Serial.print("Nachricht empfangen [");
  Serial.print(topic);
  Serial.print("]: ");
  
  // Payload in einen String umwandeln
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println(message);

  // Auswertung des Topics und der empfangenen Nachricht
  if (String(topic) == "Werktor/K0") {
    if (message == "AN") {
      Serial.println("Gerät einschalten");
      // Füge hier deinen Code zum Einschalten eines Geräts ein, z.B.:
      // digitalWrite(LED_BUILTIN, LOW);  // LED einschalten (LOW bei aktiver LOW-Logik)
    } 
    else if (message == "AUS") {
      Serial.println("Gerät ausschalten");
      // Füge hier deinen Code zum Ausschalten eines Geräts ein, z.B.:
      // digitalWrite(LED_BUILTIN, HIGH);  // LED ausschalten
    } 
    else {
      Serial.println("Unbekannter Befehl");
    }
  } else {
    Serial.println("Unbekanntes Topic");
  }

}

//************************************************************************** LOOP
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
/*
 // Senden einer Beispielnachricht an ein bestimmtes Thema
  client.publish("Werktor/K0", "Hallo von Arduino!");
  delay(5000);  // Nachricht alle 10 Sekunden senden
*/
}