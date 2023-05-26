#include <DS3231.h>
#include <Wire.h>

// Define pin constants
#define MODE_PIN 8
#define ADJUST_PIN 9
#define LED_DATA_PIN 10

// Initialise the DS3231 RTC and WS2812B LEDs
DS3231 rtc(SDA, SCL);

// Create time structure t
Time t;

// Declare enum for separate clock states
enum clockStates { CLOCK_MODE, SET_HOUR, SET_MIN, SET_DAY, SET_MONTH, SET_YEAR };
clockStates clockState;

unsigned long currentMillis; // used to limit polling of RTC module to once per second

// Define constants for seven segment displays
const int leftDigitSegA[] = { 0, 1, 2, 3, 4, 5 };
const int leftDigitSegB[] = { 5, 18, 29, 42, 53, 66 };
const int leftDigitSegC[] = { 77, 90, 101, 114, 125, 138 };
const int leftDigitSegD[] = { 138, 139, 140, 141, 142, 143 };
const int leftDigitSegE[] = { 72, 95, 96, 119, 120, 143 };
const int leftDigitSegF[] = { 0, 23, 24, 47, 48, 71 };
const int leftDigitSegG[] = { 66, 67, 68, 69, 70, 71 };

const int rightDigitSegA[] = { 6, 7, 8, 9, 10, 11 };
const int rightDigitSegB[] = { 11, 12, 35, 36, 59, 60 };
const int rightDigitSegC[] = { 83, 84, 107, 108, 131, 132 };
const int rightDigitSegD[] = { 132, 133, 134, 135, 136, 137 };
const int rightDigitSegE[] = { 78, 89, 102, 113, 126, 137 };
const int rightDigitSegF[] = { 6, 17, 30, 41, 54, 65 };
const int rightDigitSegG[] = { 60, 61, 62, 63, 64, 65 };

// Define constants for each word
const int itIs[] = { 1, 2, 4, 5 };

const int hourTwelve[] = { 121, 122, 123, 124, 125, 126 };
const int hourOne[] = { 111, 112, 113 };
const int hourTwo[] = { 115, 116, 117 };
const int hourThree[] = { 72, 73, 74, 75, 76 };
const int hourFour[] = { 103, 104, 105, 106 };
const int hourFive[] = { 84, 85, 86, 87 };
const int hourSix[] = { 127, 128, 129 }; 
const int hourSeven[] = { 78, 79, 80, 81, 82 };
const int hourEight[] = { 96, 97, 98, 99, 100 };
const int hourNine[] = { 140, 141, 142, 143 };
const int hourTen[] = { 108, 109, 110 };
const int hourEleven[] = { 89, 90, 91, 92, 93, 94 };

const int oClock[] = { 132, 133, 134, 135, 136, 137, 138 };
const int fivePast[] = { 56, 57, 58, 59, 64, 65, 66, 67 };
const int tenPast[] = { 8, 9, 10, 64, 65, 66, 67 };
const int quarterPast[] = { 15, 16, 17, 18, 19, 20, 21, 64, 65, 66, 67 };
const int twentyPast[] = { 49, 50, 51, 52, 53, 54, 64, 65, 66, 67 };
const int twentyFivePast[] = { 49, 50, 51, 52, 53, 54, 56, 57, 58, 59, 64, 65, 66, 67 };
const int halfPast[] = { 31, 32, 33, 34, 64, 65, 66, 67 };
const int twentyFiveTo[] = { 49, 50, 51, 52, 53, 54, 56, 57, 58, 59, 69, 70 };
const int twentyTo[] = { 49, 50, 51, 52, 53, 54, 69, 70};
const int quarterTo[] = { 15, 16, 17, 18, 19, 20, 21, 69, 70 };
const int tenTo[] = { 8, 9, 10, 69, 70 };
const int fiveTo[] = { 56, 57, 58, 59, 69, 70 };


void setup()
{
  // Initialise the RTC module and LEDs
  rtc.begin();

  pinMode(MODE_PIN, INPUT);
  pinMode(ADJUST_PIN, INPUT);
  
  // Put clock in CLOCK_MODE state and set to default colour
  clockState = CLOCK_MODE;
  colour = RED;
  brightness = 50;
}

void loop()
{
  switch (clockState)
  {
    case CLOCK_MODE:
      if (millis() - currentMillis >= 1000) // limits time polling to once per second
      {
        t = rtc.getTime();
        roundTime(); // rounds up to nearest minute and hour if necessary
        currentMillis = millis();
        buildTimeArray();
        updateDisplay();
      }

      if (digitalRead(MODE_PIN))
      {
        clockState = SET_HOUR;
        delay(200);
      }

      if (digitalRead(ADJUST_PIN))
      {
        colour = colour + 1;
        if (colour >= 7)
        {
          colour = 0; // wraps colour back around to zero
        }
        delay(150);
      }
      
      break;

    case SET_HOUR:
      delay(150);
      static int hourToShow = 0;
      resetLedArray();
      if (hourToShow >= 10)
      {
        buildDigitArray((hourToShow / 10) % 10, true);
        buildDigitArray(hourToShow % 10, false);
      }
      else
      {
        buildDigitArray(0, true);
        buildDigitArray(hourToShow, false);
      }
      updateDisplay();
      
      if (digitalRead(ADJUST_PIN))
      {
         delay(150);
         hourToShow++;

         if (hourToShow >= 24)
         {
            hourToShow = 0;
         }
      }

      if (digitalRead(MODE_PIN))
      {
        delay(200);
        rtc.setTime(hourToShow, 0, 0);
        clockState = SET_MIN;
      }
      break;

    case SET_MIN:
      delay(150);
      static int minToShow = 0;
      resetLedArray();
      
      if (minToShow >= 10)
      {
        buildDigitArray((minToShow / 10) % 10, true);
        buildDigitArray(minToShow % 10, false);
      }
      else
      {
        buildDigitArray(0, true);
        buildDigitArray(minToShow, false);
      }
      updateDisplay();    
      
      if (digitalRead(ADJUST_PIN))
      {
         delay(150);
         minToShow++;
         
         if (minToShow >= 60)
         {
            minToShow = 0;
         }
      }

      if (digitalRead(MODE_PIN))
      {
        t = rtc.getTime();
        rtc.setTime(t.hour, minToShow, 0);
        clockState = CLOCK_MODE;
        delay(200);
      }
      break;

    case SET_DAY:
      // To be implemented
      break;   
       
    case SET_MONTH:
      // To be implemented
      break;

    case SET_YEAR:
      // To be implemented
      break;      
  }
}  

void resetLedArray()
{
  for (int currentLed = 0; currentLed < NUM_LEDS; currentLed++) 
    {
      ledArray[currentLed] = 0;
    }
}

void roundTime() // Rounds up minutes and hours if necessary
{
  if (t.sec >= 30)
  {
    t.min = t.min + 1;
  }
  if (t.min >= 33)
  {
    t.hour = t.hour + 1;
  }
}

void updateDisplay()
{  
  int r, g, b;
  
  switch (colour)
  {
    case RED:
      r = 3;
      g = 0;
      b = 0;
      break;
      
    case GREEN:
      r = 0;
      g = 3;
      b = 0;
      break;
      
    case BLUE:
      r = 0;
      g = 0;
      b = 3;
      break;
      
    case ORANGE:
      r = 3;
      g = 2;
      b = 0;
      break;
      
    case PURPLE:
      r = 2;
      g = 0;
      b = 3;
      break;
          
    case CYAN:
      r = 0;
      g = 3;
      b = 3;
      break;
      
    case WHITE:
      r = 3;
      g = 3;
      b = 3;
      break;
      
    case RAINBOW:
      r = 1;
      g = 5;
      b = 2;
      break;
  }

  // apply brightness
  r = r * brightness;
  g = g * brightness;
  b = b * brightness;
  
  for (int currentLed = 0; currentLed < NUM_LEDS; currentLed++)
  {
    if (ledArray[currentLed] == 1)
    {
      leds[currentLed] = CRGB(r,g,b);
    }
    else
    {
      leds[currentLed] = CRGB(0,0,0);
    }
  }
  FastLED.show();
}

void buildTimeArray()
{
  resetLedArray();
  updateLedArray(itIs, 4);
  
  if (t.hour == 0 || t.hour == 12)
  {
    updateLedArray(hourTwelve, 6);
  }
  
  else if (t.hour == 1 || t.hour == 13)
  {
    updateLedArray(hourOne, 3);
  }
  
  else if (t.hour == 2 || t.hour == 14)
  {
    updateLedArray(hourTwo, 3);
  }
  
  else if (t.hour == 3 || t.hour == 15)
  {
    updateLedArray(hourThree, 5);
  }
   else if (t.hour == 4 || t.hour == 16)
  {
    updateLedArray(hourFour, 4);
  }
  
  else if (t.hour == 5 || t.hour == 17)
  {
    updateLedArray(hourFive, 4);
  }
  
  else if (t.hour == 6 || t.hour == 18)
  {
    updateLedArray(hourSix, 3);
  }
  
  else if (t.hour == 7 || t.hour == 19)
  {
    updateLedArray(hourSeven, 5);
  }
  
  else if (t.hour == 8 || t.hour == 20)
  {
    updateLedArray(hourEight, 5);
  }
  
  else if (t.hour == 9 || t.hour == 21)
  {
    updateLedArray(hourNine, 4);
  }
  
  else if (t.hour == 10 || t.hour == 22)
  {
    updateLedArray(hourTen, 3);
  }
  
  else if (t.hour == 11 || t.hour == 23)
  {
    updateLedArray(hourEleven, 6);
  }

  if (t.min < 3 || t.min > 57)
  {
    updateLedArray(oClock, 7);
  }
  
  else if (t.min > 2 && t.min < 8)
  { 
    updateLedArray(fivePast, 8);
  }
  
  else if (t.min > 7 && t.min < 13)
  { 
    updateLedArray(tenPast, 7);
  }
  
  else if (t.min > 12 && t.min < 18)
  { 
    updateLedArray(quarterPast, 11);
  }
  
  else if (t.min > 17 && t.min < 23)
  {    
    updateLedArray(twentyPast, 10);
  }
  
  else if (t.min > 22 && t.min < 28)
  {    
    updateLedArray(twentyFivePast, 14);
  }
  
  else if (t.min > 27 && t.min < 33)
  {    
    updateLedArray(halfPast, 8);
  }
  
  else if (t.min > 32 && t.min < 38)
  { 
    updateLedArray(twentyFiveTo, 12);
  }
  
  else if (t.min > 37 && t.min < 43)
  {    
    updateLedArray(twentyTo, 8);
  }
  
  else if (t.min > 42 && t.min < 48)
  {    
    updateLedArray(quarterTo, 9);
  }
  
  else if (t.min > 47 && t.min < 53)
  {
    updateLedArray(tenTo, 5);
  }
  
  else if (t.min > 52 && t.min < 58)
  {    
    updateLedArray(fiveTo, 6);
  }
}

void buildDigitArray(int numberToShow, bool leftDigit)
{
  switch (numberToShow)
  {
    case 0:
      if (leftDigit)
      {
        updateLedArray(leftDigitSegA, 6);
        updateLedArray(leftDigitSegB, 6);
        updateLedArray(leftDigitSegC, 6);
        updateLedArray(leftDigitSegD, 6);
        updateLedArray(leftDigitSegE, 6);
        updateLedArray(leftDigitSegF, 6);
      }
      else 
      {
        updateLedArray(rightDigitSegA, 6);
        updateLedArray(rightDigitSegB, 6);
        updateLedArray(rightDigitSegC, 6);
        updateLedArray(rightDigitSegD, 6);
        updateLedArray(rightDigitSegE, 6);
        updateLedArray(rightDigitSegF, 6);
      }
      break;

    case 1:
      if (leftDigit)
      {
        updateLedArray(leftDigitSegE, 6);
        updateLedArray(leftDigitSegF, 6);
      }
      else
      {
        updateLedArray(rightDigitSegB, 6);
        updateLedArray(rightDigitSegC, 6);
      }
      break;

    case 2:
      if (leftDigit)
      {
        updateLedArray(leftDigitSegA, 6);
        updateLedArray(leftDigitSegB, 6);
        updateLedArray(leftDigitSegD, 6);
        updateLedArray(leftDigitSegE, 6);
        updateLedArray(leftDigitSegG, 6);
      }
      else
      {
        updateLedArray(rightDigitSegA, 6);
        updateLedArray(rightDigitSegB, 6);
        updateLedArray(rightDigitSegD, 6);
        updateLedArray(rightDigitSegE, 6);
        updateLedArray(rightDigitSegG, 6);
      }
      break;

    case 3:
      if (leftDigit)
      {
        updateLedArray(leftDigitSegA, 6);
        updateLedArray(leftDigitSegB, 6);
        updateLedArray(leftDigitSegC, 6);
        updateLedArray(leftDigitSegD, 6);
        updateLedArray(leftDigitSegG, 6);
      }
      else
      {
        updateLedArray(rightDigitSegA, 6);
        updateLedArray(rightDigitSegB, 6);
        updateLedArray(rightDigitSegC, 6);
        updateLedArray(rightDigitSegD, 6);
        updateLedArray(rightDigitSegG, 6);
      }
      break;

    case 4:
    if (leftDigit)
      {
        updateLedArray(leftDigitSegB, 6);
        updateLedArray(leftDigitSegC, 6);
        updateLedArray(leftDigitSegF, 6);
        updateLedArray(leftDigitSegG, 6);
      }
      else
      {
        updateLedArray(rightDigitSegB, 6);
        updateLedArray(rightDigitSegC, 6);
        updateLedArray(rightDigitSegF, 6);
        updateLedArray(rightDigitSegG, 6);
      }
      break;

    case 5:
      if (leftDigit)
      {
        updateLedArray(leftDigitSegA, 6);
        updateLedArray(leftDigitSegC, 6);
        updateLedArray(leftDigitSegD, 6);
        updateLedArray(leftDigitSegF, 6);
        updateLedArray(leftDigitSegG, 6);
      }
      else
      {
        updateLedArray(rightDigitSegA, 6);
        updateLedArray(rightDigitSegC, 6);
        updateLedArray(rightDigitSegD, 6);
        updateLedArray(rightDigitSegF, 6);
        updateLedArray(rightDigitSegG, 6);
      }
      break;

    case 6:
      updateLedArray(rightDigitSegA, 6);
      updateLedArray(rightDigitSegC, 6);
      updateLedArray(rightDigitSegD, 6);
      updateLedArray(rightDigitSegE, 6);
      updateLedArray(rightDigitSegF, 6);
      updateLedArray(rightDigitSegG, 6);         
      break;

    case 7:
      updateLedArray(rightDigitSegA, 6);
      updateLedArray(rightDigitSegB, 6);
      updateLedArray(rightDigitSegC, 6);
      break;

    case 8:
      updateLedArray(rightDigitSegA, 6);
      updateLedArray(rightDigitSegB, 6);
      updateLedArray(rightDigitSegC, 6);
      updateLedArray(rightDigitSegD, 6);
      updateLedArray(rightDigitSegE, 6);
      updateLedArray(rightDigitSegF, 6);
      updateLedArray(rightDigitSegG, 6);
      break;

    case 9:
      updateLedArray(rightDigitSegA, 6);
      updateLedArray(rightDigitSegB, 6);
      updateLedArray(rightDigitSegC, 6);
      updateLedArray(rightDigitSegF, 6);
      updateLedArray(rightDigitSegG, 6);
      break;
  }
}

void updateLedArray(int inputArray[], int arraySize)
{
  for (int currentLed = 0; currentLed < arraySize; currentLed++)
  {
    ledArray[inputArray[currentLed]] = 1;
    
  }
}
