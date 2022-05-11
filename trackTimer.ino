#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

//GPIO declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
byte segmentClock = 6;
byte segmentLatch = 5;
byte segmentData = 7;

byte greenButton = 2;
byte whiteButton = 3;
int greenButtonState;
int whiteButtonState;
int laserState;
int modeStatus;

byte timeData = 8;
byte modeIn = 9;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  Serial.begin(9600);
  Serial.println("Large Digit Driver Example");

  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);

  pinMode(greenButton, INPUT);
  pinMode(whiteButton, INPUT);

  pinMode(timeData, INPUT);
  pinMode(modeIn, INPUT);

  pinMode(13, OUTPUT);

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, LOW);
  digitalWrite(segmentLatch, LOW);

}

//variable declarations
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
int seconds = 0;
float timeS;
float timeStart = millis();
int x = 0;
int milliseconds = 0;

int tenSeconds = 0;
int minutes = 0;
int tenMinutes = 0;

char* state = 'pause';

char* mode = 'buttonMode';
float modeCooldown;

bool greenButtonPressed = false;
bool whiteButtonPressed = false;

bool laserStep = false;

float runTime;
bool reset = true;

float resetMillis;
float laserResetMillis;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void loop()
{
  digitalWrite(13, HIGH);
  if (state == 'time'){
  
    timeS = millis();
  
    float fullTime = (timeS - timeStart)/1000;
  
    //TIMER SEGMENT STARTS
  
    if (seconds >= 10)
    {
      seconds = 0;
      timeStart = millis();
      tenSeconds += 1;
    }
  
    if (fullTime <= 1.00)
    {
      milliseconds = (timeS - timeStart)/10;
    }
    
    if (milliseconds >= 99)
    {
     milliseconds = 0;
     timeStart = millis();
     seconds += 1;
    }
  
    if (tenSeconds >= 6)
    {
      minutes += 1;
      tenSeconds = 0;
    }
  
    if (minutes >= 10)
    {
      tenMinutes += 1;
      minutes = 0;
    }
  }
  //TIMER SEGMENT ENDS

  showNumber(milliseconds, seconds, tenSeconds, minutes, tenMinutes); //Test pattern
  //Serial.println(seconds); //For debugging

  greenButtonState = digitalRead(greenButton);
  whiteButtonState = digitalRead(whiteButton);

  laserState = digitalRead(timeData);
  modeStatus = digitalRead(modeIn);
  char* cooldownState = 'inactive';

  if (state == 'time' && whiteButtonPressed == false){
    if (whiteButtonState == LOW){
      state = 'pause';
      whiteButtonPressed = true;
    }
    }
  if (state == 'pause' && whiteButtonPressed == false){
    if (whiteButtonState == LOW){
      state = 'time';
      whiteButtonPressed = true;
      timeStart = millis() - (milliseconds*10);
    }
  }
  
  else if (whiteButtonPressed == true){
    if (whiteButtonState == HIGH) {
      whiteButtonPressed = false;
    }

  }

  if (state == 'pause' && greenButtonPressed == false){
    if (greenButtonState == LOW){
      state = 'time';
      restartTimer();
      greenButtonPressed = true;
      resetMillis = millis();
    }
  }
  if (state == 'time' && greenButtonPressed == false){
    if (greenButtonState == LOW){
      restartTimer();
      greenButtonPressed = true;
      resetMillis = millis();
    }
  }
  else if (greenButtonPressed == true){
    if (greenButtonState == HIGH){
      greenButtonPressed = false;
      resetMillis = millis();
    }
    if (greenButtonState == LOW);
      if (millis() - resetMillis >= 2000){
        state = 'pause';
        restartTimer();
        reset = true;
      }
  }

  //LASERMODE CONDITIONALS
  //if (mode == 'laserMode'){ <-- this code is not applicable
    if (laserStep == false && reset == false){
        if (laserState == HIGH){
          if (millis() - laserResetMillis <= 1000){
            state = 'pause';
            restartTimer();
            reset = true;
          }
          laserResetMillis = millis();
          laserStep = true;
          return;
        }
      }
    if (state == 'pause' && laserStep == false && reset == true){
      if (laserState == HIGH){ 
        state = 'time';
        restartTimer();
        laserStep = true;      
      } 
    }
    if (state == 'time' && laserStep == false && reset == true){
      if (laserState == HIGH){
        state = 'pause';
        laserStep = true;
        reset = false;
      }
    }
    
    else if (laserStep == true){
      if (laserState == LOW) {
        laserStep = false;
      }
    }
}

void restartTimer()
{
  timeStart = millis();
  milliseconds = 0;
  seconds = 0;
  tenSeconds = 0;
  minutes = 0;
  tenMinutes = 0;
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber(float value, float value2, float value3, float value4, float value5)
{
  int number = abs(value); //Remove negative signs and any decimals
  int seconds = abs(value2);
  int minutes = abs(value4);
  int tenSeconds = abs(value3);
  int tenMinutes = abs(value5);
  //Serial.print("number: ");
  //Serial.println(number);

  for (byte x = 0 ; x < 2 ; x++)
  {
    int remainder = number % 10;

    postNumber(remainder, false);
    number /= 10;
  }

  for (byte x = 2 ; x < 3 ; x++)
  {
    int rem = seconds % 10;

    postNumber(rem, true);

    number /= 10;
  }

  for (byte x = 3 ; x < 4 ; x++)
  {
    int rem = tenSeconds % 10;

    postNumber(rem, false);

    number /= 10;
  }

  for (byte x = 4 ; x < 5 ; x++)
  {
    int rem = minutes % 10;

    postNumber(rem, false);

    number /= 10;
  }

  for (byte x = 5 ; x < 6 ; x++)
  {
    int rem = tenMinutes % 10;

    postNumber(rem, false);

    number /= 10;
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}
