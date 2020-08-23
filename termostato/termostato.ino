#include "Adafruit_SSD1306.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include "index.h" //HTML

const char* ssid = "xxx";
const char* password = "xxx";
const int relayPin = 16;
int screenLinesUsed = 0;

ESP8266WebServer server(80);
Adafruit_SSD1306 OLED(0);
Ticker temporizador;

int repeticiones = 0;
int contador = 0;

void setup()   {

  // Inicio el puerto del rele
  pinMode(relayPin, OUTPUT);

  // Inicio el log en el puerto serie
  Serial.begin(115200);

  // Inicio la pantalla oled
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)

  // Configuro la pantalla
  OLED.clearDisplay();
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);
  OLED.setCursor(0, 0);

  outputMSG("----------", true);
  outputMSG(" STARTUP", true);
  outputMSG("----------", true);

  // Conecto al Wifi
  outputMSG("WIFI:", false);

  //Static IP address configuration
  IPAddress staticIP(192, 168, 2, 52); //ESP static ip
  IPAddress gateway(192, 168, 2, 1);   //IP Address of your WiFi Router (Gateway)
  IPAddress subnet(255, 255, 255, 0);  //Subnet mask
  IPAddress dns(8, 8, 8, 8);  //DNS
  WiFi.config(staticIP, subnet, gateway, dns);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
  }
  outputMSG(" OK", true);
  Serial.println(WiFi.localIP().toString().c_str());

  // Arranco el servidor web
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/time", handleTime);
  server.begin();
  outputMSG("Server: OK", true);

  // Limpio la pantalla
  delay(2000);
  clearDisplay();

}

void loop() {

  server.handleClient();

}

void handleRoot() {

  server.send_P(200, "text/html", MAIN_page);

}

void handleOn() {

  //Seteo el tiempo que se va a encender la calefaccion
  contador = 0;
  repeticiones = server.arg(0).toInt();

  //Arranco el programa de la calefaccion
  calefaccion();

  server.send(200, "application/json", "{\"response\":\"OK\"}");

}

void handleTime() {

  String time = "";

  if (contador > 0) {

    time = String((repeticiones + 1) - contador);

  } else {

    time = String(repeticiones - contador);
  }

  server.send(200, "application/json", "{\"time\":" + time + "}");

}

void calefaccion() {

  //Muestro el tiempo en pantalla
  showTime(repeticiones - contador);

  //Si el tiempo ha pasado
  if (contador >= repeticiones) {

    //Pongo los contadores a 0
    contador = 0;
    repeticiones = 0;

    //Apago el rele
    digitalWrite(relayPin, LOW);

    //Muestro off en pantalla
    showOff();

    //Si el tiempo no ha pasado
  } else {

    //Enciendo el rele
    digitalWrite(relayPin, HIGH);

    //Le sumo un minuto al contador de minutos
    contador++;

    //Llamo a la funcion cada 60s
    temporizador.attach(60, calefaccion);

  }

}

void outputMSG(String msg, bool newLine) {

  if (newLine) {

    Serial.println(msg);
    OLED.println(msg);
    screenLinesUsed++;

  } else {

    Serial.print(msg);
    OLED.print(msg);

  }

  OLED.display();

}

void showTime(int time) {

  OLED.clearDisplay();
  OLED.setCursor(4, 0);
  OLED.setTextColor(WHITE);

  OLED.setTextSize(5);
  OLED.println(String(time));

  OLED.setTextSize(1);
  OLED.setCursor(5, 40);
  OLED.println(" minutes");

  OLED.display();

}

void showOff() {

  OLED.clearDisplay();
  OLED.setCursor(5, 15);
  OLED.setTextColor(WHITE);
  OLED.setTextSize(3);
  OLED.println("OFF");
  OLED.display();

  temporizador.attach(5, clearDisplay);

}

void clearDisplay() {

  OLED.clearDisplay();
  OLED.display();

}
