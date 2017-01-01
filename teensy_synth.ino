////////////////////////////////////////////////////////////////////////
//          Headers
///////////////////////////////////////////////////////////////////////

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>



////////////////////////////////////////////////////////////////////////
//          Define Symbolic Constatns
///////////////////////////////////////////////////////////////////////

#define DEBUG 

//definitions for the pins going to the mux to scan through the 8 columns
//these are outputs
#define SEL_A 2
#define SEL_B 1
#define SEL_C 0

//pin definitions for the row inputs
#define ROW_1 4
#define ROW_2 5
#define ROW_3 6
#define ROW_4 7
#define ROW_5 8
#define ROW_6 10
#define ROW_7 12


#define NUM_KEYS    49
#define NUM_COLUMNS 8
#define NUM_ROWS    7

#define KEY_ON      0xff
#define KEY_OFF     0x00


////////////////////////////////////////////////////////////////////////
//          Globals
///////////////////////////////////////////////////////////////////////
// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=525,341
AudioOutputI2S           i2s1;           //xy=1015,346
AudioConnection          patchCord1(sine1, 0, i2s1, 0);
AudioConnection          patchCord2(sine1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=911,616
// GUItool: end automatically generated code

unsigned long keyStateLow  = 0;
unsigned long keyStateHigh = 0;
const byte keyMidiValues[NUM_ROWS][NUM_COLUMNS]= {  {0,1,2,3,4,5,6,7},
                                                    {8,9,10,11,12,13,14,15},
                                                    {16,17,18,19,20,21,22,23},
                                                    {24,25,26,27,28,29,30,31},
                                                    {32,33,34,35,36,37,38,39},
                                                    {40,41,42,43,44,45,46,47},
                                                    {48,255,255,255,255,255,255,255}};
                                              
byte keyTable[NUM_ROWS][NUM_COLUMNS];
byte oldKeyTable[NUM_ROWS][NUM_COLUMNS];


int potPin = A0;
int potValue = 0;

////////////////////////////////////////////////////////////////////////
//          setup()
///////////////////////////////////////////////////////////////////////
void setup() 
{

  //default oscillators to off (applitude = 0)
  AudioMemory(8);
  sine1.amplitude(0);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);

  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  
  //init ketable arrays to 0's
  for(byte colCnt = 0; colCnt <8; colCnt++)
  {
    for(byte rowCnt = 0; rowCnt < 7; rowCnt++)
    {
      keyTable[rowCnt][colCnt] = 0;
      oldKeyTable[rowCnt][colCnt] = 0;
    }
  }
  //mux sel outputs
  pinMode(SEL_A, OUTPUT);
  pinMode(SEL_B, OUTPUT);
  pinMode(SEL_C, OUTPUT);

  //row inputs
  pinMode(ROW_1, INPUT_PULLUP);
  pinMode(ROW_2, INPUT_PULLUP);
  pinMode(ROW_3, INPUT_PULLUP);
  pinMode(ROW_4, INPUT_PULLUP);
  pinMode(ROW_5, INPUT_PULLUP);
  pinMode(ROW_6, INPUT_PULLUP);
  pinMode(ROW_7, INPUT_PULLUP);

}

////////////////////////////////////////////////////////////////////////
//          main() loop
///////////////////////////////////////////////////////////////////////
void loop() 
{
  for(byte colCnt = 0; colCnt <8; colCnt++)
  {
    
    cycleColumns(colCnt);
    delay(1);
    for(byte rowCnt = 0; rowCnt < 7; rowCnt++)
    {
      if(checkRow(rowCnt))
      {
        keyTable[rowCnt][colCnt] = KEY_ON;
      }
      else
      {
        keyTable[rowCnt][colCnt] = KEY_OFF;
      }
      if(keyTable[rowCnt][colCnt] != oldKeyTable[rowCnt][colCnt])
      {
        oldKeyTable[rowCnt][colCnt] = keyTable[rowCnt][colCnt];
        if(keyTable[rowCnt][colCnt] == KEY_ON)
        {
          #ifdef DEBUG
          Serial.print(keyMidiValues[rowCnt][colCnt],DEC);
          Serial.print(" is on\r\n");
          #endif
          //send note to osc?
          sine1.frequency(mtof(60 + keyMidiValues[rowCnt][colCnt]));
          sine1.amplitude(1);
        }
        else
        {
          #ifdef DEBUG
          Serial.print(keyMidiValues[rowCnt][colCnt],DEC);
          Serial.print(" is off\r\n");
          #endif
          //kill note from osc?
          sine1.amplitude(0);
        }
      }
    }
  }

  potValue = readPot(potPin);
  
}
void cycleColumns(byte colNum)
{
  switch(colNum)
  {
    case 0:
      digitalWriteFast(SEL_A, LOW);  
      digitalWriteFast(SEL_B, LOW);  
      digitalWriteFast(SEL_C, LOW);
      break;
    case 1:  
      digitalWriteFast(SEL_A, HIGH);  
      digitalWriteFast(SEL_B, LOW);  
      digitalWriteFast(SEL_C, LOW);
      break;
    case 2: 
      digitalWriteFast(SEL_A, LOW);  
      digitalWriteFast(SEL_B, HIGH);  
      digitalWriteFast(SEL_C, LOW);  
      break;
    case 3:
      digitalWriteFast(SEL_A, HIGH);  
      digitalWriteFast(SEL_B, HIGH);  
      digitalWriteFast(SEL_C, LOW);
      break;
    case 4:  
      digitalWriteFast(SEL_A, LOW);  
      digitalWriteFast(SEL_B, LOW);  
      digitalWriteFast(SEL_C, HIGH); 
      break;
    case 5:
      digitalWriteFast(SEL_A, HIGH);  
      digitalWriteFast(SEL_B, LOW);  
      digitalWriteFast(SEL_C, HIGH);
      break;
    case 6:
      digitalWriteFast(SEL_A, LOW);  
      digitalWriteFast(SEL_B, HIGH);  
      digitalWriteFast(SEL_C, HIGH);
      break;
    case 7:
      digitalWriteFast(SEL_A, HIGH);  
      digitalWriteFast(SEL_B, HIGH);  
      digitalWriteFast(SEL_C, HIGH); 
      break;
    default:
      digitalWriteFast(SEL_A, LOW);  
      digitalWriteFast(SEL_B, LOW);  
      digitalWriteFast(SEL_C, LOW);
      break;
  }
}
bool checkRow(byte rowNum)
{
  switch(rowNum)
  {
    case 0:
      if (digitalReadFast(ROW_1)== 0)
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
    case 1:
      if (digitalReadFast(ROW_2)== 0)
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
    case 2:
      if (digitalReadFast(ROW_3)== 0)
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
    case 3:
      if (digitalReadFast(ROW_4)== 0)
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
   case 4:
      if (digitalReadFast(ROW_5)== 0)
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
    case 5:
      if (digitalReadFast(ROW_6)== 0)
      {
        return true;
      }
      else
      {
        return false;
      }
      break;
    
    case 6:
      if (digitalReadFast(ROW_7)== 0)
      {
        return true;
      }
      else
      {
        return false;
      }
      break;   
    default:
      return false;
      break;
  }
}
float mtof(byte midiNoteVal)
{
  float freq = 0;
  freq = 440 * pow(2.0,(midiNoteVal-69.0)/12.0);
  #ifdef DEBUG
  Serial.println(freq);
  #endif
  return freq;
}
int readPot(int pot)
{
  int x; 
  x = analogRead(pot);
  #ifdef DEBUG
  if (x != potValue)
  {
    Serial.print("Pot Value: ");
    Serial.println(x);
  }
  #endif
  return x;
}

