// inny OLED 1,3" + WWW
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
//#include <LiquidCrystal_I2C.h>
//#include <Adafruit_GFX.h>
//#include "SSD1306Wire.h"
#include "SH1106Wire.h"
//Encoder myEnc(D6, D7);

const char* ssid = "TP";
const char* password = "21";
WiFiServer server(80);// Set port to 80
String header; // This storees the HTTP request

#define RX D5
#define TX D6

SoftwareSerial pzemSWSerial(RX, TX);
PZEM004Tv30 pzemL1(pzemSWSerial, 0x01);
PZEM004Tv30 pzemL2(pzemSWSerial, 0x02);
PZEM004Tv30 pzemL3(pzemSWSerial, 0x03);

//LiquidCrystal_I2C lcd(0x27,20,4);
//SSD1306Wire  display(0x3c, D2, D1);
SH1106Wire display(0x3c, D2, D1);

OneWire oneWire(D4); //Podłączenie do A5
DallasTemperature sensors(&oneWire); //Przekazania informacji do biblioteki

int maxNapiecie = 244.00;

int temp = 0;
int tempMAX = 80;

int SSR_L1 = D7;
int SSR_L2 = D8;
int SSR_L3 = D0;

int wartosc1 = 0;
float napiecieL1 = 0;

int wartosc2 = 0;
float napiecieL2 = 0;

int wartosc3 = 0;
float napiecieL3 = 0;

int wartoscA1 = 0;
float natezenieL1 = 0;

int wartoscA2 = 0;
float natezenieL2 = 0;

int wartoscA3 = 0;
float natezenieL3 = 0;

//float test = 0;


void setup() {
Serial.begin(115200);

  display.init();
  display.flipScreenVertically();
  sensors.begin(); //Inicjalizacja czujnika temp

//connect to access point
WiFi.begin(ssid, password);
Serial.print("Podłączono do :  ");
Serial.println(ssid);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
// Print local IP address and start web server
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());// this will display the Ip address of the Pi which should be entered into your browser
server.begin();
  
  pinMode(SSR_L1, OUTPUT);
  digitalWrite(SSR_L1, LOW);

  pinMode(SSR_L2, OUTPUT);
  digitalWrite(SSR_L2, LOW);

  pinMode(SSR_L3, OUTPUT);
  digitalWrite(SSR_L3, LOW);
   
pinMode(LED_BUILTIN, OUTPUT);

}
void loop()

{
WiFiClient client = server.available(); // Listen for incoming clients
if (client) { // If a new client connects,
String currentLine = ""; // make a String to hold incoming data from the client
while (client.connected()) { // loop while the client's connected
if (client.available()) { // if there's bytes to read from the client,
char c = client.read(); // read a byte, then
Serial.write(c); // print it out the serial monitor
header += c;
if (c == '\n') { // if the byte is a newline character
if (currentLine.length() == 0) {
client.println("HTTP/1.1 200 OK");
client.println("Content-type:text/html");
client.println("Connection: close");
client.println();

    napiecieL1 = pzemL1.voltage();
    napiecieL2 = pzemL2.voltage();
    napiecieL3 = pzemL3.voltage();

    natezenieL1 = pzemL1.current();
    natezenieL2 = pzemL2.current();
    natezenieL3 = pzemL3.current();

  sensors.requestTemperatures(); //Pobranie temperatury czujnika
  temp = sensors.getTempCByIndex(0);

  //test = analogRead(A0);
  //Serial.print("ADC : ");
  //Serial.println(test);  //Wyswietlenie informacji  
  Serial.print("Temperatura MAX: ");
  Serial.println(tempMAX);  //Wyswietlenie informacji
  Serial.print("Temperatura wody: ");
  Serial.println(temp);  //Wyswietlenie informacji

 if (temp < tempMAX){
    pomiar();
  }
  else { 
    pomiar();
    delay(1000);
    cieplaWoda();
    delay(1000);
  } 
//  grafikaStala();
//  delay(1000);
//  cieplaWoda();
//  delay(1000);
//}


// Display the HTML web page

client.println("<!DOCTYPE html><html>");

client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

client.println("<link rel=\"icon\" href=\"data:,\">");

// CSS to style the on/off buttons

// Feel free to change the background-color and font-size attributes to fit your preferences

client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");

client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");

client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");

client.println(".button2 {background-color: #77878A;}</style></head>");


// Web Page Heading

client.println("<body><h1>ESP8266 Web Server</h1>");



//wyswietla V i A
client.println("<p>Temperatura CWU: " + String(temp) + " C* " + "</p>");
client.println("<p> </p>");
client.println("<p>Faza 1 (L1): " + String(napiecieL1) + " V " + natezenieL1 + " A " + "</p>");
client.println("<p>Faza 2 (L2): " + String(napiecieL2) + " V " + natezenieL2 + " A " + "</p>");
client.println("<p>Faza 3 (L3): " + String(napiecieL3) + " V " + natezenieL1 + " A " + "</p>");

client.println("<p> </p>");
client.println("<p>Napiecie zalaczania: " + String(maxNapiecie) + " V " + "</p>");
client.println("<p>Natezenie zalaczania: " + String(temp) + " A " + "</p>");
client.println("<p> </p>");
client.println("<p>Temperatura max CWU: " + String(tempMAX) + " C* " + "</p>");
//client.println("<input type="textbox" value="wprowadz" id="text"> </input>");

//Ustawienia


client.println("</body></html>");
// The HTTP response ends with another blank line

client.println();

// Break out of the while loop

break;

} else { // if you got a newline, then clear currentLine

currentLine = "";

}

} else if (c != '\r') { // if you got anything else but a carriage return character,

currentLine += c; // add it to the end of the currentLine

}}

}


// Clear the header variable

header = "";

// Close the connection

client.stop();

Serial.println("Client disconnected.");

Serial.println("");

}


}
void pomiar() {

  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
//=======================================================================
//                    TEMPERATURA
//=======================================================================
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "TEMP CWU:");

  display.setFont(ArialMT_Plain_16);
  display.drawString(75, 0, String(temp));
  
  display.setFont(ArialMT_Plain_16);
  display.drawString(110, 0, "*C");
//=======================================================================
//                    FAZA 1
//=======================================================================  
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 16, "L1:");
  display.setFont(ArialMT_Plain_16);
  display.drawString(78, 16, "V");

  if(isnan(napiecieL1)){
        Serial.println("Blad odczytu na fazie L1");
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(26, 16, "off");
    } else {
        Serial.print("Napiecie L1: ");      Serial.print(napiecieL1);      Serial.println("V");
        display.setFont(ArialMT_Plain_16);
        display.drawString(24, 16, String(napiecieL1));   
    }
//------- L1 -------//
if (napiecieL1 >= maxNapiecie && temp < tempMAX) {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(96, 16, "-ON-");
        digitalWrite(SSR_L1, HIGH );
        Serial.println("L1---- X ");}
  else {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(100, 16, "--");
        digitalWrite(SSR_L1, LOW );
        Serial.println("L1---- O ");}    
//=======================================================================
//                    FAZA 2
//=======================================================================     
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 32, "L2:");
  display.setFont(ArialMT_Plain_16);
  display.drawString(78, 32, "V");

  if(isnan(napiecieL2)){
        Serial.println("Blad odczytu na fazie L2");
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(26, 32, "off");
   } else {
        Serial.print("Napiecie L2: ");      Serial.print(napiecieL2);      Serial.println("V");
        display.setFont(ArialMT_Plain_16);
        display.drawString(24, 32, String(napiecieL2));   
    }
//------- L2 -------//
if (napiecieL2 >= maxNapiecie && temp < tempMAX) {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(96, 32, "-ON-");
        digitalWrite(SSR_L2, HIGH );
        Serial.println("L2---- X ");}
  else {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(100, 32, "--");
        digitalWrite(SSR_L2, LOW );
        Serial.println("L2---- O ");}
//=======================================================================
//                    FAZA 3
//=======================================================================   
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 49, "L3:");
  display.setFont(ArialMT_Plain_16);
  display.drawString(78, 49, "V");

  if(isnan(napiecieL2)){
        Serial.println("Blad odczytu na fazie L3");
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(26, 49, "off");
   } else {
        Serial.print("Napiecie L3: ");      Serial.print(napiecieL3);      Serial.println("V");
        display.setFont(ArialMT_Plain_16);
        display.drawString(24, 49, String(napiecieL3));   
    }
//------- L3 -------//
if (napiecieL3 >= maxNapiecie && temp < tempMAX) {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(96, 49, "-ON-");
        digitalWrite(SSR_L3, HIGH );
        Serial.println("L3---- X ");}
  else {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(100, 49, "--");
        digitalWrite(SSR_L3, LOW );
        Serial.println("L3---- O ");}   
  display.display();
}

void cieplaWoda() {
  
  display.clear(); // clear the display
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_16);
  display.drawString(30, 0, "UWAGA");

  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 16, "TEMP CWU");  
  display.setFont(ArialMT_Plain_24);
  display.drawString(40, 39, String(temp));
   
  display.display();
}

