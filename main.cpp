#include <LowPower.h>
#include <avr/power.h>
#include <Wire.h>

//Interval
#define INTERVAL 2

//GLOBAL
#define LED_PIN 13

//BUTTON
#define BUTTON 2

//TIME
#define PCF8563address 0x51
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
byte alarmMinute, alarmHour, alarmDay, alarmDayOfWeek;

byte bcdToDec(byte value){
 return ((value / 16) * 10 + value % 16);
}

byte decToBcd(byte value) {
 return (value / 10 * 16 + value % 10);
}

void readPCF8563() {
 Wire.beginTransmission(PCF8563address);
 Wire.write(0x02);
 Wire.endTransmission();
 Wire.requestFrom(PCF8563address, 7);
 second = bcdToDec(Wire.read() & B01111111); 
 minute = bcdToDec(Wire.read() & B01111111); 
 hour = bcdToDec(Wire.read() & B00111111);
 dayOfMonth = bcdToDec(Wire.read() & B00111111);
 dayOfWeek = bcdToDec(Wire.read() & B00000111);
 month = bcdToDec(Wire.read() & B00011111); 
 year = bcdToDec(Wire.read());
}

void setPCF8563()
// this sets the time and date to the PCF8563
{
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.write(decToBcd(second));  
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));     
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(dayOfWeek));  
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

void setPCF8563alarm()
// this sets the alarm data to the PCF8563
{
  byte am, ah, ad, adow;
  am = decToBcd(alarmMinute);
  am = am | 100000000; // set minute enable bit to on
  ah = decToBcd(alarmHour);
  ah = ah | 100000000; // set hour enable bit to on
  ad = decToBcd(alarmDay);
  ad = ad | 100000000; // set day of week alarm enable bit on
  adow = decToBcd(alarmDayOfWeek);
  adow = ad | 100000000; // set day of week alarm enable bit on

  // write alarm data to PCF8563
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x09);
  Wire.write(am);  
  Wire.write(ah);

  // optional day of month and day of week (0~6 Sunday - Saturday)
  /*
  Wire.write(ad);
  Wire.write(adow);  
  */
  Wire.endTransmission();

  // optional - turns on INT_ pin when alarm activated  
  // will turn off once you run void PCF8563alarmOff()
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x01);
  Wire.write(B00000010);
  Wire.endTransmission();
}

void setPCF8563nextalarm(int interval)
{
 alarmMinute = minute + interval;
 alarmHour = hour;
 if (alarmMinute > 60){
   alarmMinute -= 60;
   alarmHour += 1;
 }
 if (alarmHour > 23){
   alarmHour = 0;
 }
 setPCF8563alarm();
}

void PCF8563alarmOff()
// turns off alarm enable bits and wipes alarm registers. 
{
  byte test;
  // first retrieve the value of control register 2
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address, 1);
  test = Wire.read();

  // set bit 3 "alarm flag" to 0
  test = test - B00001000;

  // now write new control register 2  
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x01);
  Wire.write(test);
  Wire.endTransmission();
}

void wakeUp(){
 // Just a handler for the pin interrupt.
 //while (!digitalRead(BUTTON)) {};
}

void blink(unsigned int sec){
  for (size_t i = 0; i < sec*6; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void setup() {
 pinMode(BUTTON, INPUT_PULLUP);
 pinMode(LED_PIN, OUTPUT);
 pinMode(0, OUTPUT);
 pinMode(1, OUTPUT);
 pinMode(3, OUTPUT);
 pinMode(4, OUTPUT);
 pinMode(5, OUTPUT);
 pinMode(6, OUTPUT);
 pinMode(7, OUTPUT);
 pinMode(8, OUTPUT);
 pinMode(9, OUTPUT);
 pinMode(10, OUTPUT);
 pinMode(11, OUTPUT);
 pinMode(12, OUTPUT);

 digitalWrite(0, LOW);
 digitalWrite(1, LOW);
 digitalWrite(3, LOW);
 digitalWrite(4, LOW);
 digitalWrite(5, LOW);
 digitalWrite(6, LOW);
 digitalWrite(7, LOW);
 digitalWrite(8, LOW);
 digitalWrite(9, LOW);
 digitalWrite(10, LOW);
 digitalWrite(11, LOW);
 digitalWrite(12, LOW);
 digitalWrite(LED_PIN, HIGH);

 Wire.begin();

// change the following to set your initial time
 second = 50;
 minute = 44;
 hour = 13;
 dayOfWeek = 1;
 dayOfMonth = 19;
 month = 8;
 year = 13;
// comment out the next line and upload again to set and keep the time from resetting every reset
// setPCF8563();

 readPCF8563();

 setPCF8563nextalarm(INTERVAL);

 delay(1000);

 digitalWrite(LED_PIN, LOW);

 blink(4);
}

void loop(){
 attachInterrupt(0, wakeUp, LOW);
 LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
 detachInterrupt(0);

 PCF8563alarmOff();

 blink(10);

 readPCF8563();

 setPCF8563nextalarm(INTERVAL);
}
