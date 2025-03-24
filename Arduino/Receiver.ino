#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <HTTPClient.h>
#include "time.h"

#define ss 5         // LoRa chip select pin
#define rst 14       // LoRa reset pin
#define dio0 4       // LoRa data pin

#define TFT_CS 15    // TFT chip select pin
#define TFT_DC 2     // TFT DC or data/command pin
#define TFT_RST 12   // TFT reset pin

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

const char * ssid = "Accio_WiFi";
const char * password = "Accio_Ritam09";

//==================================GOOGLE SHEET CREDENTIALS=========================================
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "---***YOUR GOOGLE SCRIPT API ID***---";

//===================================================================================================
byte localAddress = 0xF5;
int recipient;
String incoming;
byte sender;
byte incomingMsgId;
byte incomingLength;

struct Data
{
  float Wind_spd;
  String Wind_dir;
  float Temp;
  float Humid;
  float HeatI;
  int Lux;
  int UV;
  float Pres;
  float Batt;
}data;

//========================================DISPLAY FUNCTION===========================================

void updatedisplay()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.print(int(data.Batt));
  tft.print("%");
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(60, 10);
  tft.println("Weather");

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(60, 40);
  tft.println("Insight");

  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.print("Wind Spd:");
  tft.setTextColor(ILI9341_YELLOW);
  tft.print(int(data.Wind_spd));
  tft.println(" kmph");
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 110);
  tft.print("Wind Dir: ");
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(data.Wind_dir);
//  tft.println(" deg");
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 140);
  tft.print("Temperature:");
  tft.setTextColor(ILI9341_YELLOW);
  tft.print(data.Temp);
  tft.println(" C");
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 170);
  tft.print("Humidity: ");
  tft.setTextColor(ILI9341_YELLOW);
  tft.print(data.Humid);
  tft.println(" %");

  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 200);
  tft.print("Heat Index: ");
  tft.setTextColor(ILI9341_YELLOW);
  tft.print(data.HeatI);
  tft.println(" C");
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 230);
  tft.print("Light: ");
  tft.setTextColor(ILI9341_YELLOW);
  tft.print(data.Lux);
  tft.println(" lux");
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 260);
  tft.print("UV: ");
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(data.UV);
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 290);
  tft.print("Pressure:");
  tft.setTextColor(ILI9341_YELLOW);
  tft.print(data.Pres);
  tft.println("hPa");
  
//  tft.setTextColor(ILI9341_CYAN);
//  tft.setTextSize(2);
//  tft.setCursor(10, 260);
//  tft.print("SENDER: ");
//  tft.setTextColor(ILI9341_YELLOW);
//  tft.print("0x");
//  tft.println(sender, HEX);
//  
//  tft.setTextColor(ILI9341_CYAN);
//  tft.setTextSize(2);
//  tft.setCursor(10, 290);
//  tft.print("RSSI: ");
//  tft.setTextColor(ILI9341_YELLOW);
//  tft.println(LoRa.packetRssi());
}
//==============================================================================================

void setup() 
{
  Serial.begin(115200);
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  
  Serial.println("LoRa Receiver");
  LoRa.setPins(ss, rst, dio0);  
  if (!LoRa.begin(433E6)) 
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  //--------------------------------------------------------------------------------------------------
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.flush();
  //-----------------------------------------------------------------------------------------------
}

void loop() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;
  
  recipient = LoRa.read();
  sender = LoRa.read();
  incomingMsgId = LoRa.read();
  incomingLength = LoRa.read();
  LoRa.readBytes((byte *)&data, packetSize);
  
  incoming = String(data.Wind_spd) + String(data.Wind_dir) + String(data.Temp) + String(data.Humid) + String(data.HeatI) + String(data.Lux) + String(data.UV) + String(data.Pres) + String(data.Batt);
  
//  if (incomingLength != incoming.length()) 
//  {
//    Serial.println(incoming.length());
//    Serial.println("error: message length does not match length");
//    return;
//  }

  if (recipient != localAddress) 
  {
    Serial.println("This message is not for me.");
    return;
  }
  
  Serial.println("**********************************************************************");
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));

  Serial.println("Wind_spd: " + String(data.Wind_spd));
  Serial.println("Wind_dir: " + String(data.Wind_dir));
  Serial.println("Temp: " + String(data.Temp));
  Serial.println("Humid: " + String(data.Humid));
  Serial.println("Heat Index: " + String(data.HeatI));
  Serial.println("Lux: " + String(data.Lux));
  Serial.println("UV: " + String(data.UV));
  Serial.println("Pres: " + String(data.Pres));
  Serial.println("Battery: " + String(data.Batt));

  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println("");

  //===================================================================================================
  if (WiFi.status() == WL_CONNECTED) 
  {
      static bool flag = false;
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) 
      {
        Serial.println("Failed to obtain time");
        return;
      }

//      float temper = random(29.0, 40.0);
//      float hum = random(25.0, 80.0);
//      float heati = random(29.0, 50.0);
//      int uv = random(280, 350);
//      float presr = random(1000.0, 1050.0);
//      int light = random(0, 65536);
//      float sped = random(0.0, 80.0);
//      int dirc = random(0, 7);
//      int battery = random(0, 100);

      
      String temp      = String(data.Temp);
      String humid     = String(data.Humid);
      String heatI     = String(data.HeatI);
      String UV        = String(data.UV);
      String pres      = String(data.Pres);
      String light_int = String(data.Lux);
      String spd       = String(data.Wind_spd);
      String dir       = String(data.Wind_dir);
      String batt      = String(data.Batt); //******//


      String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?temp=" + temp + "&humid=" + humid + "&heatI=" + heatI + 
                         "&UV=" + UV + "&pres=" + pres + "&light=" + light_int + "&w_spd=" + spd + "&w_dir=" + dir + "&batt=" + batt;
      
      Serial.print("POST data to spreadsheet:");
//      Serial.println(urlFinal);
      HTTPClient http;
      http.begin(urlFinal.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET(); 
      Serial.print("HTTP Status Code: ");
      Serial.println(httpCode);
      //---------------------------------------------------------------------
      //getting response from google sheet
//      int payload;
//      if (httpCode > 0) {
//          payload = http.getSize();
//          Serial.println(payload);    
//      }
      //---------------------------------------------------------------------
      http.end();
  }
  
  updatedisplay();
}
