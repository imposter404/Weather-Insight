//==============================================LIBRARIES=================================================
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_LTR390.h"
#include "DHT.h"
#include <SPI.h>
#include <LoRa.h>
#include <CD74HC4067.h>

#define windsensor 26 
#define ss 5
#define rst 14
#define dio0 2

#define DHTPIN 32     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define SEALEVELPRESSURE_HPA (1013.25)
#define ALTITUDE 11.0

//============================================PIN DEFINITION==============================================
Adafruit_ADS1115 ads;
BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_LTR390 ltr = Adafruit_LTR390();
Adafruit_BME280 bme;

// s0 s1 s2 s3
CD74HC4067 my_mux(4, 16, 17, -1);
const int sig_in = 35; //33
unsigned long previousMillis = 0;

float divider_ratio = 0.180328;  //R1=10k & R2=2.2k
float VSI = 0.00;
float batteryPercent = 0.00;
float BatteryMin = 3.0;
float BatteryMax = 14.4;

int m[8];
String wind_dir;
float temp = 0.00;
float hum = 0.00;
float pres = 0.00;
int lastState = 0;
int counter = 0; 

int windMap[2][32] = {{2000,1400,1000,600,500,400,320,280,240,220,200,180,170,160,150,140,130,120,110,100,90,80,70,60,50,40,38,36,34,32,30,28},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,20,23,25,29,34,41,51,54,57,60,64,68,73}};

byte msgCount = 0;
byte localAddress = 0xB5;
byte destination = 0xF5;

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

//============================================SENSOR FUNCTIONS==============================================
//float getbatt()
//{
//  for(int i = 0; i<10; i++)
//  {
//    VSI = VSI + ads.computeVolts(ads.readADC_SingleEnded(0));
//  }
//  
//  float voltageInput  = (VSI/10)*divider_ratio;
//  batteryPercent  = ((voltageInput - BatteryMin)/(BatteryMax - BatteryMin))*101;
//  return batteryPercent  = constrain(batteryPercent,0,100);
//  //  return voltageInput;      
//}

float gettemp()
{
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  return t;
}

float gethumid()
{
  float h = dht.readHumidity();
  Serial.print(F("Humidity: "));
  Serial.println(h);
  return h;
}

float getheatI()
{
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temp, hum, false);
  Serial.print(hic);
  Serial.println(F("°C "));
  return hic;
}

int getlux()
{
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  return lux;
}

float getpres()
{
  Serial.print("Pressure = ");
  float pressure = bme.readPressure();
//  pressure = bme.seaLevelForAltitude(ALTITUDE, pressure);
  Serial.print(pressure/100.0);
  Serial.println(" hPa");
  return pressure/100.0;
}

//float getalti()
//{
//  float alt = bme.readAltitude(pres);
//  Serial.print("Approx. Altitude = ");
//  Serial.print(alt);
//  Serial.println(" m");
//  return alt;
//}

int getLTR()
{
  if (ltr.newDataAvailable()) 
  {
      Serial.print("UV data: "); 
      Serial.println(ltr.readUVS());
  }
  return ltr.readUVS();
}

float getspeed()
{
  int ave = 2000;
  if(counter > 1)                                                    //Check that at least two ticks have been registered, one is meaningless
  {
    ave = 6000/(counter-1);                                          //Calculate the average time between ticks
  }
  Serial.print("Average Tick Time: ");
  Serial.println(ave);
  if (ave < 28)                                                      //Discard readings faster than 102kph
    ave = 28;
  int index = 0;
  while (ave < windMap[0][index])                                    //Search through the windMap array for the corressponding wind value
    index++;
  float wind = windMap[1][index];
  Serial.print("Wind Speed: ");
  Serial.println(wind);
  return wind;
}

String getdir()
{
  int dir = 0;
  String wind_dir;
  for (int i = 0; i < 8; i++)
  {
    if(m[i] == 0)
      dir = i;
  }

  switch(dir)
      {
        case 0:
          wind_dir = "N";
          break;
        case 1:
          wind_dir = "NW";
          break;
        case 2:
          wind_dir = "W";
          break;
        case 3:
          wind_dir = "SW";
          break;
        case 4:
          wind_dir = "S";
          break;
        case 5:
          wind_dir = "SE";
          break;
        case 6:
          wind_dir = "E";
          break;
        case 7:
          wind_dir = "NE";
          break;
        default:
          wind_dir = "--";
      }
  Serial.println(wind_dir);
  return wind_dir;
}
//======================================================================================================================================================================

void setup() 
{
  Serial.begin(115200);
  pinMode(sig_in, INPUT);
  pinMode(windsensor, INPUT);
  
  //*********************************************************************************************************  
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);  
  if (!LoRa.begin(433E6)) 
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");

  //*********************************************************************************************************
  ads.setGain(GAIN_ONE);
  ads.begin();
  
  //*********************************************************************************************************
  Wire.begin();

  lightMeter.begin();
  dht.begin();

  //*********************************************************************************************************
  bool status;
  status = bme.begin(0x76);  
  if (!status) 
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  //***************************************************************************************************************************
  if ( ! ltr.begin() ) 
  {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setMode(LTR390_MODE_UVS);
  ltr.setGain(LTR390_GAIN_3);
  ltr.setResolution(LTR390_RESOLUTION_16BIT);
  ltr.setThresholds(100, 1000);
  ltr.configInterrupt(true, LTR390_MODE_UVS);
  
  //***************************************************************************************************************************
  
}

void loop() 
{
  temp = gettemp();
  hum = gethumid();
//  getheatI();
//  getlux();
  pres = getpres();
//  getalti();
//  getLTR();

  for (int i=0; i<= 6000; i++)
  {
    if (digitalRead(windsensor) == LOW && lastState == 0)
    {
      counter++;
      lastState = 1;
    }
    else if (digitalRead(windsensor) == HIGH && lastState == 1)
    {
      lastState = 0;
    }
    delay(1);
  }
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 10) 
  {
    // save the last time
    previousMillis = currentMillis;  
    
    for (int i = 0; i < 8; i++)
    {
      my_mux.channel(i);
      int val1 = analogRead(sig_in);
      m[i] = val1;
    }

    wind_dir = getdir();
  }

  Serial.print("Sending packet: ");
  Serial.println(msgCount);

  data.Wind_spd = getspeed();
  data.Wind_dir = wind_dir;
  data.Temp = gettemp();
  data.Humid = gethumid(); //62.4
  data.HeatI = getheatI();
  data.Lux = getlux(); //1570.0
  data.UV = getLTR(); //10.0
  data.Pres = getpres(); //1012.8
  data.Batt = 83.00;
  
  String outgoing = String(data.Wind_spd) + String(data.Wind_dir) + String(data.Temp) + String(data.Humid) + String(data.HeatI) + String(data.Lux) + String(data.UV) + String(data.Pres) + String(data.Batt);
  
  // send packet
  LoRa.beginPacket();
  LoRa.write(destination);
  LoRa.write(localAddress);
  LoRa.write(msgCount);
  LoRa.write(outgoing.length());
  LoRa.write((byte *)&data, sizeof(data));

  LoRa.endPacket();
  msgCount++;

  Serial.println(outgoing.length());
  delay(500);
}
