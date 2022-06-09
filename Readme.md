# Thief Detecter
 When your treasure is being taken away, making a Line notify(or E-mail) to let you know.<br>
 You also get your location info on the arduino ide screen.<br>
  * Line Notify<br>
![image](https://github.com/MarseusFu/Embedded-System/blob/master/line%20notify.jpg)
  * E-mail<br>
![image](https://github.com/MarseusFu/Embedded-System/blob/master/Email.jpg)
## Components and supplies
 1. ESP 32
 2. Force Sensor
 3. Resistor 3.3k ohm
 4. Jumper wires
 5. neo 6m GPS module

## Wiring
 * Force Sensor
![image](https://github.com/MarseusFu/Embedded-System/blob/master/Wiring/ForceSensor.jpg)

 * GPS
![image](https://github.com/MarseusFu/Embedded-System/blob/master/Wiring/GPS.jpg)

## 待改進
 * Force sensor感測不穩定，誤觸可能導致讀取數值錯誤，以為東西被拿走了。
 * GPS模組要在室外取得位置的能力較佳，在室內啟動來取得位置需很長時間，原因是室內有遮蔽物所以很難讀取到衛星訊號，且若模組沒電了，過往讀取紀錄消失，下次取得位置又要花很長時間。
 * GPS功能雞肋，目前作法只是取得失竊位置，若可以裝在物品上，於失竊後追蹤其位置更有實際效用。

## References
 * Send E-mails with ESP32 - [Link](https://www.mischianti.org/2020/06/16/send-email-with-attachments-emailsender-v2-x-library-esp32-and-esp8266-part-2/#ESP32)<br>
 * ESP32 installation and configuring development environment - [Link](https://www.mischianti.org/2020/06/16/send-email-with-attachments-emailsender-v2-x-library-esp32-and-esp8266-part-2/#ESP32)<br>
 * ESP32 - Force Sensor  - [Link](https://esp32io.com/tutorials/esp32-force-sensor)<br>