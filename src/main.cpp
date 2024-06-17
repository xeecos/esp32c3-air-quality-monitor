#include <Arduino.h>
#include "config.h"
#include <HardwareSerial.h>
#include <SGP40.h>
#include <Wire.h>
#include <AHTxx.h> 

// #include <WiFi.h>

// const char *ssid = "makeblock.cc";
// const char *password = "hulurobot";

// WiFiServer server(80);
SGP40 sgp;
float ahtValue;                               //to store T/RH result
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
}
int prevC = 0;
bool startFrame = false;
int i = 0;
uint8_t buf[32];
void parseBuf();
void loop()
{
    digitalWrite(PIN_LED,LOW);
    delay(1000);
    digitalWrite(PIN_LED,HIGH);
    delay(1000);
    uint16_t voc;
    voc = sgp.getVoclndex();
    USBSerial.printf("Measurement: %d\n", voc);

    ahtValue = aht21.readTemperature(); //read 6-bytes via I2C, takes 80 milliseconds

    
    if (ahtValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
    {
        USBSerial.printf("Temperature: %f\n", ahtValue);
    }
    else
    {

        if   (aht21.softReset() == true) USBSerial.println(F("reset success")); //as the last chance to make it alive
        else                             USBSerial.println(F("reset failed"));
    }

    ahtValue = aht21.readHumidity(); //read another 6-bytes via I2C, takes 80 milliseconds

    
    if (ahtValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
    {
        USBSerial.printf("Humidity: %f\n",ahtValue);
    }
    else
    {
    }
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
                parseBuf();
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
void parseBuf()
{
    int pm1 = (buf[2]<<8)+buf[3];
    int pm25 = (buf[4]<<8)+buf[5];
    int pm10 = (buf[6]<<8)+buf[7];
    USBSerial.printf("pm1.0:%d pm2.5:%d pm10:%d\n",pm1, pm25, pm10);

    // int um3 = (buf[14]<<8)+buf[15];
    // int um5 = (buf[16]<<8)+buf[17];
    // int um10 = (buf[18]<<8)+buf[19];
    // int um25 = (buf[20]<<8)+buf[21];
    // int um50 = (buf[22]<<8)+buf[23];
    // int um100 = (buf[24]<<8)+buf[25];
    // USBSerial.printf("um0.3:%d um0.5:%d um1.0:%d um2.5:%d um5.0:%d um10.0:%d\n",um3, um5, um10,um25,um50,um100);
}