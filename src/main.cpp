
 #include <RTClib.h>
#include <Wire.h>
 
 RTC_DS3231 rtc;

 #define CLOCK_INTERRUPT_PIN RTC_INT
 
 void setup() {
     Serial.begin(115200);
 
     // initializing the rtc
     if(!rtc.begin()) {
         Serial.println("Couldn't find RTC!");
         Serial.flush();
         while (1) delay(10);
     }
 
     if(rtc.lostPower()) {
         rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     }

     rtc.disable32K();
     pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
     attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);
     rtc.clearAlarm(1);
     rtc.clearAlarm(2);
     rtc.writeSqwPinMode(DS3231_OFF);
     rtc.disableAlarm(2);
 

     if(!rtc.setAlarm1(
             rtc.now() + TimeSpan(10),
             DS3231_A1_Second // this mode triggers the alarm when the seconds match. See Doxygen for other options
     )) {
         Serial.println("Error, alarm wasn't set!");
     }else {
         Serial.println("Alarm will happen in 10 seconds!");
     }
 }
 
 void loop() {
     // print current time
     char date[10] = "hh:mm:ss";
     rtc.now().toString(date);
     Serial.print(date);
 
     // the stored alarm value + mode
     DateTime alarm1 = rtc.getAlarm1();
     Ds3231Alarm1Mode alarm1mode = rtc.getAlarm1Mode();
     char alarm1Date[12] = "DD hh:mm:ss";
     alarm1.toString(alarm1Date);
     Serial.print(" [Alarm1: ");
     Serial.print(alarm1Date);
     Serial.print(", Mode: ");
     switch (alarm1mode) {
       case DS3231_A1_PerSecond: Serial.print("PerSecond"); break;
       case DS3231_A1_Second: Serial.print("Second"); break;
       case DS3231_A1_Minute: Serial.print("Minute"); break;
       case DS3231_A1_Hour: Serial.print("Hour"); break;
       case DS3231_A1_Date: Serial.print("Date"); break;
       case DS3231_A1_Day: Serial.print("Day"); break;
     }
 
     // the value at SQW-Pin (because of pullup 1 means no alarm)
     Serial.print("] SQW: ");
     Serial.print(digitalRead(CLOCK_INTERRUPT_PIN));
 
     // whether a alarm fired
     Serial.print(" Fired: ");
     Serial.print(rtc.alarmFired(1));
 
     if (rtc.alarmFired(1)) {
         rtc.clearAlarm(1);
         Serial.print(" - Alarm cleared");
     }
     Serial.println();
 
     delay(2000);
 }
 
 void onAlarm() {
     Serial.println("Alarm occured!");
 }