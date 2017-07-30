#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "TP-LINK_886D";
const char* password = "54122024";

volatile double pressure;
volatile int count;
#define TEXT_LENGTH 200
char text[TEXT_LENGTH];

void timer1_ISR(void){
  pressure = 0.95*pressure + 0.05*analogRead(A0);
  count++;
  timer0_write(ESP.getCycleCount() + 800000L); 
}

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  snprintf(text, TEXT_LENGTH, "%d", (int)pressure);
  server.send(TEXT_LENGTH, "text/plain", text);
  digitalWrite(led, 0);
  count = 0;
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  count = 0;
  pressure = analogRead(A0);
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer1_ISR);
  timer0_write(ESP.getCycleCount() + 800000L); // 80MHz == 1sec
  interrupts();
  
}

void loop(void){
  server.handleClient();
}
