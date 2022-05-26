/*
 * EMailSender library for Arduino, esp8266 and esp32
 * Simple esp32 Gmail send example
 *
 * https://www.mischianti.org/category/my-libraries/emailsender-send-email-with-attachments/
 *
 */
 
#include "Arduino.h"
#include <EMailSender.h>
#include <WiFi.h>
#include "time.h"
 
const char* ssid = "DESKTOP-EU9UVT8 8433";
const char* password = "12345678";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7*3600;
const int daylightOffset_sec = 3600;
char timeStringBuff[50];

uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;
 
EMailSender emailSend("marseus891130@gmail.com", "rnqgmtccbicujxyz","ESP32 Secure");
 
uint8_t WiFiConnect(const char* nSSID = ssid, const char* nPassword = password)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);
        Serial.println(nSSID);
    }
 
    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}
 
void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(timeStringBuff,sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
void setup()
{
    Serial.begin(115200);
 
    connection_state = WiFiConnect(ssid, password);
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect
    
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
    
    EMailSender::EMailMessage message;
    message.subject = "Alert your treasure is being taken away";
    String TimeString = "Time : "+String(timeStringBuff);
    message.message = TimeString+"<br>Hello<br>bonjour<br>i love you";
 
    EMailSender::Response resp = emailSend.send("marseus891130@gmail.com", message);
 
    Serial.println("Sending status: ");
 
    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);

    
}
 
void loop()
{
 
}
