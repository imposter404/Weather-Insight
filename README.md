# Weather-Insight

<div align="left">

<img src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/javascript/javascript-original.svg" height="50px" alt="JavaScript"  />


<img src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/html5/html5-original.svg" height="50px" alt="Html"  />
          

<img src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/cplusplus/cplusplus-original.svg" height="50px" alt="C++" />  

<img src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/arduino/arduino-original-wordmark.svg" height="50px" alt="Arduino" />  
    
</div>
          





## Overview
This project is a weather monitoring system built using the **ESP32** microcontroller. 
It collects various environmental data such as `Temperature`, `Humidity`, `Heat Index` ,`Gas Detector` and displays the information in real-time with data visualization






## Description:
It is a **Hardware** & **Software** based project basically a **IoT** based project. 
It can be done using any type of hardware controller. Eg: `Arduino`, `ESP`, `Raspberry Pi`.
Here we have used **ESP32** module 




## Why ESP32 ?
**ESP32** is a low cost microcontroller mostly used in IOT based project. It comes with built-in `WiFi` And `Bluetooth module` 
- **Connectivity**: ESP32 has built-in module 
  - Wi-Fi 
  - Bluetooth 
- **I/O port**:The microcontroller includes a variety of peripherals such as:
  - GPIO (General Purpose Input/Output) pins
  - ADC (Analog to Digital Converter)
  - DAC (Digital to Analog Converter)
  - PWM (Pulse Width Modulation)
- **Low Power Consumption**
- **Development Environments** : Arduino IDE





## Features
- **Real-time Data Monitoring**: Continuously measures and displays `Temperature`, `Humidity`, `Heat Index` and harmful `Gas`.
- **Database**: **`ESP32`** Sends data to a web server or here we have used `Google Sheets` as a database to store and retrieve historical data .
- **Interface**: Displays data on the website or a web dashboard. Data are being fetch from database (here `Google Sheets`).
- **Data Logging**: Stores historical data on database for **`Data Analysis`** and **`Data Mining`**.



# Project Requirements
## Hardware
- ESP32 
- Breadboard
- Wires
- AC to DC adaptor 
- **Sensors**
  - **Temperature, Humidity, Heat Index**
    - DHT11
  - **Gas Director**
    - MQ7
    - MQ8
    - MQ135



## Software 
- Webpage 
- Google Sheets (Database)
- Google App console (retrieve data from ESP32)
- Arduino IDE 
- and a internet connection :)


# Code 
## Arduino

<img src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/cplusplus/cplusplus-original.svg" height="50px" alt="C++" />  


> ``` C++
> #include<iostream>
> #include <SPI.h>
> #include <LoRa.h>
> #include <WiFi.h>
> #include <SPI.h>
> #include <Wire.h>
> #include <Adafruit_GFX.h>
> #include <Adafruit_ILI9341.h>
> #include <HTTPClient.h>
> #include "time.h"
> ```

Arduino code are at [`Arduino`](Arduino/Readme.md) folder 

> [!NOTE]
> Change the URL in the code with your Google Sheets URL



## Front-End 

<img src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/javascript/javascript-original.svg" height="50px" alt="JavaScript"  />

JS Library 
- Apex Chart JS 

> ```Js 
> <script src="apexChartJS.js"></script>
> ```


# Working 

