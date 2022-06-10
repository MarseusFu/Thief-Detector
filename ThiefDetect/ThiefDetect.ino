#include "Arduino.h"
#include <EMailSender.h>
#include <WiFi.h>
#include "time.h"
#define FORCE_SENSOR_PIN 36 // ESP32 pin GIOP36 (ADC0): the FSR and 10K pulldown are connected to A0
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
//----------------------------
const char* ssid = "DESKTOP-EU9UVT8 8433";//網路名稱
const char* password = "12345678";//網路密碼
String Linetoken = "YOUR LINE TOKEN";//跟LINE申請
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;// The TinyGPSPlus object
SoftwareSerial ss(RXPin, TXPin);// The serial connection to the GPS device
//----------------------------
WiFiClientSecure client;//網路連線物件
char host[] = "notify-api.line.me";//LINE Notify API網址


const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7*3600;
const int daylightOffset_sec = 3600;
char timeStringBuff[50];

uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;
 
EMailSender emailSend("marseus891130@gmail.com", "rnqgmtccbicujxyz","ESP32 Secure");
EMailSender::EMailMessage message;

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
    //連線成功
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    client.setInsecure();
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



void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  
  connection_state = WiFiConnect(ssid, password);
  if(!connection_state)  // if not connected to WIFI
       Awaits();          // constantly trying to connect

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

}

void loop() {
  int analogReading = analogRead(FORCE_SENSOR_PIN);

  /*Serial.print("The force sensor value = ");
  Serial.print(analogReading); // print the raw analog reading
  Serial.print("\n");*/
  
  if(ss.available()>0)
    if(gps.encode(ss.read()))
      displayInfo();

  if(millis()>5000 && gps.charsProcessed()<10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
  
  if (analogReading > 0);
  //  Serial.println(" -> safe");
  else //被拿走
  {
    //送email
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

    message.subject = "Alert! your treasure is being taken away";
    String TimeString = "Time : "+String(timeStringBuff);
    /*message.message = TimeString+"<br>Your stuff has been taken away";
    
    EMailSender::Response resp = emailSend.send("marseus891130@gmail.com", message);
    Serial.println("Sending status: ");
    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);*/
    //---------------------------

    //觸發Line Notify
    String message = "\n"+TimeString;
    message+="\nAlert! Your stuff has been taken away!\n";
    if (client.connect(host, 443)) {
      int LEN = message.length();
      //傳遞POST表頭
      String url = "/api/notify";
      client.println("POST " + url + " HTTP/1.1");
      client.print("Host: "); client.println(host);
      //權杖
      client.print("Authorization: Bearer "); client.println(Linetoken);
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: "); client.println( String((LEN + 8)) );
      client.println();      
      client.print("message="); client.println(message);
      client.println();
      //等候回應
      delay(2000);
      String response = client.readString();
      //顯示傳遞結果
      Serial.println(response);
      client.stop(); //斷線，否則只能傳5次
    }
    else {
      //傳送失敗
      Serial.println("connected fail");
    }
  }
    
  //每5秒讀取一次
  //delay(5000);
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
