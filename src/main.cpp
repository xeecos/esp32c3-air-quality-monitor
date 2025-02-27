#include <Arduino.h>
#include "config.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <HardwareSerial.h>
#include <SGP40.h>
#include <Wire.h>
#include <AHTxx.h> 

// #include <WiFi.h>

// const char *ssid = "makeblock.cc";
// const char *password = "hulurobot";

// WiFiServer server(80);
SGP40 sgp;                          //to store T/RH result
AHTxx aht21(AHTXX_ADDRESS_X38, AHT2x_SENSOR); //sensor address, sensor type
HardwareSerial uart(0);
void setup()
{
    USBSerial.begin(115200);
    uart.begin(9600, SERIAL_8N1);
    pinMode(PIN_LED, OUTPUT);
    while(sgp.begin(/*duration = */10000) !=true){
        USBSerial.println("Sensor not found :(");
        delay(1000);
    }
    while (aht21.begin() != true)
    {
        delay(1000);
    } 
    
    USBSerial.println("Sensor start!");
    // USBSerial.println();
    // USBSerial.println();
    // USBSerial.print("Connecting to ");
    // USBSerial.println(ssid);

    // WiFi.mode(WIFI_AP_STA);
    // WiFi.softAP("makeblock", password);
    // WiFi.begin(ssid, password);

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     USBSerial.println(".");
    // }

    // USBSerial.println("");
    // USBSerial.println("WiFi connected.");
    // USBSerial.println("IP address: ");
    // USBSerial.println(WiFi.localIP());

    // server.begin();
     SPIClass spi(FSPI);
    spi.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
    if (!SD.begin(PIN_CS, spi, 20000000))
    {
        USBSerial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        USBSerial.println("No SD card attached");
        return;
    }

    USBSerial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        USBSerial.println("MMC");
    } else if(cardType == CARD_SD){
        USBSerial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        USBSerial.println("SDHC");
    } else {
        USBSerial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    USBSerial.printf("SD Card Size: %lluMB\n", cardSize);
}
int prevC = 0;
bool startFrame = false;
int i = 0;
uint8_t buf[32];
void parseBuf(int voc,float tempValue,float humValue);
void loop()
{
    digitalWrite(PIN_LED,LOW);
    delay(500);
    digitalWrite(PIN_LED,HIGH);
    delay(500);
    uint16_t voc;
    voc = sgp.getVoclndex();

    float tempValue = aht21.readTemperature(); //read 6-bytes via I2C, takes 80 milliseconds
    
    if (tempValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
    {
    }
    else
    {
        aht21.softReset();
    }

    float humValue = aht21.readHumidity(); //read another 6-bytes via I2C, takes 80 milliseconds
    
    sgp.setRhT(humValue, tempValue);

    while(uart.available())
    {
        char c = uart.read();
        if(c==0x4d && prevC == 0x42)
        {
            startFrame = true;
            i = 0;
        }
        else if(startFrame)
        {
            buf[i] = c;
            i++;
            if(i==30)
            {
                parseBuf(voc,tempValue,humValue);
            }
        }
        prevC = c;
    }
    // WiFiClient client = server.available(); // listen for incoming clients

    // if (client)
    // {                                  // if you get a client,
    //     USBSerial.println("New Client."); // print a message out the serial port
    //     String currentLine = "";       // make a String to hold incoming data from the client
    //     while (client.connected())
    //     { // loop while the client's connected
    //         if (client.available())
    //         {                           // if there's bytes to read from the client,
    //             char c = client.read(); // read a byte, then
    //             USBSerial.write(c);        // print it out the serial monitor
    //             if (c == '\n')
    //             { // if the byte is a newline character

    //                 // if the current line is blank, you got two newline characters in a row.
    //                 // that's the end of the client HTTP request, so send a response:
    //                 if (currentLine.length() == 0)
    //                 {
    //                     // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    //                     // and a content-type so the client knows what's coming, then a blank line:
    //                     client.println("HTTP/1.1 200 OK");
    //                     client.println("Content-type:text/html");
    //                     client.println();

    //                     // the content of the HTTP response follows the header:
    //                     client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
    //                     client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

    //                     // The HTTP response ends with another blank line:
    //                     client.println();
    //                     // break out of the while loop:
    //                     break;
    //                 }
    //                 else
    //                 { // if you got a newline, then clear currentLine:
    //                     currentLine = "";
    //                 }
    //             }
    //             else if (c != '\r')
    //             {                     // if you got anything else but a carriage return character,
    //                 currentLine += c; // add it to the end of the currentLine
    //             }

    //             // Check to see if the client request was "GET /H" or "GET /L":
    //             if (currentLine.endsWith("GET /H"))
    //             {
    //                 digitalWrite(PIN_LED, HIGH); // GET /H turns the LED on
    //             }
    //             if (currentLine.endsWith("GET /L"))
    //             {
    //                 digitalWrite(PIN_LED, LOW); // GET /L turns the LED off
    //             }
    //         }
    //     }
    //     // close the connection:
    //     client.stop();
    //     USBSerial.println("Client Disconnected.");
    // }
}
void parseBuf(int voc,float tempValue,float humValue)
{
    int pm1 = (buf[2]<<8)+buf[3];
    int pm25 = (buf[4]<<8)+buf[5];
    int pm10 = (buf[6]<<8)+buf[7];
    USBSerial.printf("%d %d %d %d %.1f %.1f\n",pm1, pm25, pm10, voc, tempValue, humValue);

    // int um3 = (buf[14]<<8)+buf[15];
    // int um5 = (buf[16]<<8)+buf[17];
    // int um10 = (buf[18]<<8)+buf[19];
    // int um25 = (buf[20]<<8)+buf[21];
    // int um50 = (buf[22]<<8)+buf[23];
    // int um100 = (buf[24]<<8)+buf[25];
    // USBSerial.printf("um0.3:%d um0.5:%d um1.0:%d um2.5:%d um5.0:%d um10.0:%d\n",um3, um5, um10,um25,um50,um100);
}
