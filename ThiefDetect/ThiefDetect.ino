//#include "SPI.h"
//#include "Phpoc.h"
//
//PhpocEmail email;
//PhpocDateTime datetime;
#include "Arduino.h"
#include <EMailSender.h>
#include <WiFi.h>

const char* ssid = "DESKTOP-EU9UVT8 8433";
const char* password = "12345678";
 
uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;

const int THRESHOLD = 50;
boolean objPresent = false;
long adcValue = 0;

boolean sendGmail();

EMailSender emailSend("marseus891130@gmail.com", "rnqgmtccbicujxyz");
 
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
void setup() {
//    Serial.begin(9600);
    Serial.begin(115200);
    while(!Serial)
    ;
    pinMode(A0, INPUT); // to read value from sensor
    pinMode(5, OUTPUT); // to control relay
    
//    Phpoc.begin(PF_LOG_SPI | PF_LOG_NET | PF_LOG_APP);
//    datetime.date("Y-M-d H:i:s");
    EMailSender::EMailMessage message;
    message.subject = "Alert! Your treasure is being taken away";
    message.message = "Hello<br>bonjour<br>i love you";

}

void loop() {
    // read 30 time and get average value to eliminate noises
    adcValue = 0;
    
    for(int i = 0; i < 30; i++)
    {
        adcValue += analogRead(A0);
        delay(10);
    }
    
    adcValue /= 30;
    
    if(adcValue < THRESHOLD)
    {
        
        if(objPresent)
        {
            //Alert
             digitalWrite(5, HIGH);
            // send notification
            while(!sendGmail())
               ;
            
            objPresent = false;
        }
    }
    else
    {
        objPresent = true;
        digitalWrite(5, LOW);
    }
}

boolean sendGmail(){
      EMailSender::Response resp = emailSend.send("marseus891130@gmail.com", message);
 
      Serial.println("Sending status: ");
 
      Serial.println(resp.status);
      Serial.println(resp.code);
      Serial.println(resp.desc);
//    // setup outgoing relay server - gmail.com
//    email.setOutgoingServer("smtp.gmail.com", 587);
//    email.setOutgoingLogin("your_account", "your_password");
//    
//    // setup From/To/Subject
//    email.setFrom("your_account@gmail.com", "Arduino");
//    email.setTo("receiver_account@gmail.com", "Boss");
//    
//    email.setSubject("Alert: Your treasure is being taken away");
//    
//    // write email message
//    email.beginMessage();
//    email.println("Location: Planet Earth.");
//    email.println("");
//    email.print("Time: ");
//    email.println(datetime.date());
//    email.println("");
//    email.println("Your treasure is being taken away.");
//    email.endMessage();
//    
//    // send email
//    if(email.send() > 0)
//    {
//        Serial.println("Email send ok");
//        return true;
//    }
//    else
//    {
//        Serial.println("Email send failed");
//        return false;
//    }
}
