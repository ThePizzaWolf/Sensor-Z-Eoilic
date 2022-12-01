// Creado ChepeCarlos de ALSW
// Tutorial Completo en https://nocheprogramacion.com
// Canal Youtube https://youtube.com/alswnet?sub_confirmation=1


#if defined(ESP32)
//Librerias para ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;

#elif defined(ESP8266)
//Librerias para ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

#endif

#include "data.h"

int pinLed = 2;
int Estado;
const int analogPin = A0;
float voltaje = 2.6;              //valorA0 * (5.0/1023.0);


const uint32_t TiempoEsperaWifi = 5000;

unsigned long TiempoActual = 0;
unsigned long TiempoAnterior = 0;
const long TiempoCancelacion = 500;

WiFiServer servidor(80);

void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando multi Wifi");

  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, 0);

  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);

  WiFi.mode(WIFI_STA);
  Serial.print("Conectando a Wifi ..");
  while (wifiMulti.run(TiempoEsperaWifi) != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println(".. Conectado");
  Serial.print("SSID:");
  Serial.print(WiFi.SSID());
  Serial.print(" ID:");
  Serial.println(WiFi.localIP());

  servidor.begin();

}

void loop() {
  WiFiClient cliente = servidor.available();

  if (cliente) {
    Serial.println("Nuevo Cliente");
    TiempoActual = millis();
    TiempoAnterior = TiempoActual;
    String LineaActual = "";

    while (cliente.connected() && TiempoActual - TiempoAnterior <= TiempoCancelacion) {
      if (cliente.available()) {
        TiempoActual = millis();
        char Letra = cliente.read();
        if (Letra == '\n') {
          if (LineaActual.length() == 0) {
            ResponderCliente(cliente);
            break;
          } else {
            Serial.println(LineaActual);
            LineaActual = "";
          }
        }  else if (Letra != '\r') {
          LineaActual += Letra;
        }
      }
    }

    cliente.stop();
    Serial.println("Cliente Desconectado");
    Serial.println();
  }

}

void ResponderCliente(WiFiClient& cliente) {
  if (voltaje >= 1.25 && voltaje <2.5 ){
    Serial.println("NO HAY VOLTAJE");
    Estado = false;
  } else if (voltaje >= 2.5 && voltaje <3.75) {
    Serial.println("Existe voltaje");
    Estado = true;
}
  cliente.print("HTTP/1.1 200 OK\n");
  cliente.print("Content-Type: text/html\n\n");
  cliente.print(Pagina);
  cliente.print("<br>Estado del Generador: ");
  cliente.print(Estado ? "Existe voltaje" : "NO HAY VOLTAJE"); 

}
