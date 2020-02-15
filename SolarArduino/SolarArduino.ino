#include <Servo.h>
#include "U8glib.h"            // U8glib library for the OLED
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

///////////////

///////////////
#define OLED_MOSI 11
#define OLED_CLK 13
#define OLED_DC    4
#define OLED_CS    8
#define OLED_RESET 10

U8GLIB_SSD1306_128X64 u8g(OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC, OLED_RESET); //sw

int analogInput = 0;
float vout = 0.0;
float vin = 0.0;
float R1 = 100000.0; // resistance of R1 (100K)
float R2 = 10000.0; // resistance of R2 (10K) 
int value = 0;

Servo servo;   // Create a servo object to control the servo
int eLDRPin = A4; // Assign pins to the LDR's
int wLDRPin = A5;
int eastLDR = 0; //Create variables to store to LDR readings
int westLDR = 0;
int difference = 0; //Create a variable to compare the two LDR's
int error = 10;  // Variable for is there is a noticable difference between the tow LDR's
int servoSet = 130; //Variable for position of servo - will be different for each device


void setup() {
  servo.attach(9);   //attaches the servo object to PWM pin 9
  Serial.begin(115200); 
  mySerial.begin(115200);
  // pinMode(analogInput, INPUT);
}

void loop() {
  eastLDR = analogRead(eLDRPin); //Read the LDR values
  westLDR = analogRead(wLDRPin);

  // read the value at analog input
  value = analogRead(analogInput);
  vout = (value * 5.0) / 1024.0; // see text
  vin = vout / (R2/(R1+R2)); 
  if (vin<0.09) 
  {
    vin=0.0;//statement to quash undesired reading !  
  } 
  
  u8g.firstPage();  
  do 
  {
    draw();
    
  }
  while( u8g.nextPage() );
  delay(500);
  writeFloat(vin);
//
//  if (eastLDR < 400 && westLDR < 400) {  //Check to see if there is low light on both LDR's
//    while (servoSet <=140 && servoSet >=15) {    // if so, send panels back to east for the sunrise
//      servoSet ++;
//      servo.write(servoSet);
//      delay(100);
//    }
//  }

  difference = eastLDR - westLDR ; //Check the difference 
  if (difference > 10) {          //Send the panel towards the LDR with a higher reading
    if (servoSet <= 180) {
      servoSet += 15;
      servo.write(servoSet);
    }
  } else if (difference < -10) {
    if (servoSet >= 0) {
      servoSet -= 15;
      servo.write(servoSet);
    }
  }
  Serial.print(eastLDR);      //Serial monitor can be useful for debugging/setting up
  Serial.print("   -   ");    //Use it to see if your LDR's are noticeably different when
  Serial.print(westLDR);      //They have equal light shining on them, if so, correct with the error value
  Serial.print("   -   ");
  Serial.print(difference);   
  Serial.print("   -   ");
  Serial.print(servoSet);     //Fine tune the servo settings, to maximise swing available
  Serial.print("   -   ");
  Serial.println(".");
  delay(100);
}
void draw(void) 
{
  u8g.setFont(u8g_font_profont17r);        // select font
  u8g.drawStr(18, 12, "VOLTAGE"); 
  u8g.setPrintPos(33,40);
  u8g.drawRFrame(15, 20, 100, 30, 10);     // draws frame with rounded edges
  u8g.println(vin);                        //Prints the voltage
  u8g.println("V");
}


void writeFloat(float x)
{
//  char idString[] = "mofeed";
//  
//  mySerial.write((uint8_t*)&idString[0], 6);
  uint8_t xxxx = x * 5;
  
  mySerial.write(xxxx);
}
