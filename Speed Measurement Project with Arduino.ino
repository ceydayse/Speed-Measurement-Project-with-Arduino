#include <SPI.h>
#include<LiquidCrystal.h> //LCD Display library
#include<virtuabotixRTC.h>
#include <SD.h>
LiquidCrystal lcd(8, 7, 6, 5, 4, 3); // RS, E, D4, D5, D6, D7 pin connections of LCD Screen
const byte LM393=2; //FC-33 Speed sensor data pin (Interrupt Pin, INT 0)
unsigned int rpm=0; //revolutions per minute
float hiz=0; //Km/h
volatile byte sayac =0;
unsigned long timeold=0;
unsigned int encoder=20; //Number of holes on the encoder disk
const int tekerlek = 70; //The diameter of the wheel is 70 mm
static volatile unsigned long debounce=0; //To solve capacity effect with code
int pwm=10; //Enable signal for speed control
int pot=A0; //Speed control potentiometer connection
virtuabotixRTC myRTC(8, 9, 10); // We pin our library to pins

File myFile;

// change this to match your SD shield or module;
const int chipSelect = 10;
void setup(){
lcd.begin(16,2); //LCD Display is initialized
pinMode(LM393,INPUT); //FC-33 Speed sensor data pin input
attachInterrupt(digitalPinToInterrupt(LM393),counter,RISING); //Counting on the rising edge of the signal
sayac=0;
rpm=0;
timeold=0;
pinMode(pwm,OUTPUT);
pinMode(pot,INPUT);


Serial.begin(9600);                                   //We initiate serial communication.
//myRTC.setDS1302Time(00,13, 11, 11, 05, 11, 2022);     //We set the time in seconds, minutes, hours, day of the week, day of the month, month, year.

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("HIZ:");
    myFile.print("RPM=");
    myFile.print(rpm,DEC);
    myFile.print(" devir/dk");
    myFile.println("HIZ=");
    myFile.print(hiz,2);
    myFile.print(" km/saat");
    myRTC.updateTime(); //Read time from RTC
    myFile.print("DATE:");
    myFile.print(myRTC.month); //Print the month on the screen.
    myFile.print("/");
    myFile.print(myRTC.dayofmonth); //Print the day on the screen.
    myFile.print("/");
    myFile.print(myRTC.year); //Print the year on the screen.
    myFile.print("TIME:");
    myFile.print(myRTC.hours); //Print the hour on the screen.
    myFile.print(":");
    myFile.print(myRTC.minutes); //Print the minute on the screen.   
    myFile.print(":");
    myFile.print(myRTC.seconds); //Print the second on the screen.
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }


}
void loop(){

myRTC.updateTime(); //Read time from RTC

  lcd.clear(); //Before we begin, we clean the screen.
  lcd.print("DATE:");
  lcd.setCursor(6, 0); //Start typing from the first row, first column of the screen.
  lcd.print(myRTC.month); //Print the month on the screen.
  lcd.print("/");
  lcd.print(myRTC.dayofmonth); //Print the day on the screen.
  lcd.print("/");
  lcd.print(myRTC.year); //Print the year on the screen.
  lcd.setCursor(0, 1); //We start typing from the 2nd row, 1st column of the LCD.
  lcd.print("TIME:");
  lcd.setCursor(6, 1); 
  lcd.print(myRTC.hours); //Print the hour on the screen.
  lcd.print(":");
  lcd.print(myRTC.minutes); //Print the minute on the screen.
  lcd.print(":");
  lcd.print(myRTC.seconds); //Print the second on the screen.
  delay(1000); //Wait 1 second
  if (millis()-timeold>=1000){
    noInterrupts();
    rpm=(60UL*1000/encoder)/(millis()-timeold)*sayac; //Calculation of RPM
    hiz=rpm*3.1416*tekerlek*60/1000000; //Calculation of speed
    timeold=millis();

  ///lcd clock info
  
  lcd.clear();
  lcd.home();
  
  
  lcd.print("RPM=");
  lcd.print(rpm,DEC);
  lcd.print(" devir/dk");
  lcd.setCursor(0,1);
  lcd.print("HIZ=");
  lcd.print(hiz,2);
  lcd.print(" km/saat");
  sayac=0;
  interrupts();
  }
  int pot_deger=analogRead(pot); //Value read from speed control pot
  int pwm_deger=map(pot_deger, 0, 1023, 0, 255); //Converting 10-bit data to 8-bit data
  analogWrite(pwm,pwm_deger); //PWM signal output

  Serial.print("RPM=");
  Serial.print(rpm,DEC);
  Serial.print(" devir/dk");
  Serial.println("HIZ=");
Serial.print(hiz,2);
Serial.print(" km/saat");
  delay(1000);
 // delay(5000);
}

  
void counter(){
if (digitalRead(LM393) && (micros()-debounce>500) && digitalRead(LM393)){ //Capacity effect of 500 microseconds
debounce = micros();
sayac++;} //Counting operation
else ;}
