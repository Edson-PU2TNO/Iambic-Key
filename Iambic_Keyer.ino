// Simple Iambic Keyer v1.00
// by Ernest PA3HCM
// Changed by PU2TNO

#define P_DOT    2   // Connects to the dot lever of the paddle
#define P_DASH   3   // Connects to the dash lever of the paddle

#include <Adafruit_RGBLCDShield.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
int speed = 60; //20 WPM
//    int speed = 120; // 10 WPM
unsigned long t0,t1,t2,t3=0;
boolean flag, flag2 = false;
char* lookupString = ".EISH5..4..V...3..UF........?_2..ARL.......+.*...WP....@.J...1'.TNDB6.-=.%X/.....KC....;!Y%.....MGZ7....,Q......O.8:.....9..0..";
byte currentDecoderIndex = 0;
byte currentDashJump = 128;
char currentAssumedChar='\0';
byte curPos = 0;

byte icons[6][8] = { { 0x04,0x0e,0x15,0x04,0x04,0x04,0x04 }, // UP
                     { 0x04,0x04,0x04,0x04,0x15,0x0e,0x04 }, // DOWN
                     { 0x10,0x18,0x18,0x00,0x07,0x02,0x02 }, // BT
                     { 0x10,0x18,0x18,0x00,0x05,0x06,0x05 }, // BK
                     { 0x14,0x18,0x14,0x00,0x19,0x15,0x13 }, // KN
                     { 0x18,0x10,0x18,0x08,0x1d,0x06,0x05 } // SK
};

//! Enum of backlight colors.
enum Icons {UP=0x00, DOWN, BT, BK, KN, SK};

// Initializing the Arduino
void setup()
{
  lcd.begin(16,2);

// Creating LCD chars
  lcd.createChar(UP, icons[UP]);
  lcd.createChar(DOWN, icons[DOWN]);  
  lcd.createChar(BT, icons[BT]); 
  lcd.createChar(BK, icons[BK]); 
  lcd.createChar(KN, icons[KN]); 
  lcd.createChar(SK, icons[SK]);

  lcd.cursor();
  lcd.clear();
  lcd.print (1200/speed);
  lcd.print(" WPM");
  lcd.setCursor(0,1);
  pinMode(P_DOT,INPUT_PULLUP);
  pinMode(P_DASH,INPUT_PULLUP );
  Serial.begin(9600);

}

// Main routine
void loop()
{

  if ((lcd.readButtons() & BUTTON_LEFT) || (!digitalRead(P_DOT))) // If the dot lever is presssed..     
  {
    keyAndBeep(speed);            // ... send a dot at the given speed
    currentAssumedChar = lookup('.');
    delay(speed);
    t2 = millis(); 
    flag = true;
    flag2 = false;
  }
  

  if ((lcd.readButtons() & BUTTON_RIGHT) || (!digitalRead(P_DASH))) // If the dash lever is pressed...      
  {
    keyAndBeep(speed*3);         // ... send a dash at the given speed
    currentAssumedChar = lookup('-');
    delay(speed);
    t2 = millis();
    flag = true;
    flag2 = false;
  }

  if ((millis()-t2 > speed) & flag)
  { 
    t0 = millis();
    lcd.print(currentAssumedChar);
    curPos++;
    if (curPos == 16)
    {  
      lcd.setCursor(0,1);
      curPos = 0;      
    }  
    lookup('\0');
    flag = false;
    t1 = millis()-t0;
    delay (2*speed-t1);
    t3 = millis();
    flag2=true;
  }


  if (((millis()-t3) > (13*speed)) & flag2)
  {
    lcd.print(" ");
    curPos++;
    if (curPos == 16)
    {  
      lcd.setCursor(0,1);
      curPos = 0;      
    }
   flag2 = false;   
   t3 = 0;      
  }
}
// Key the transmitter and sound a beep
void keyAndBeep(int speed)
{
  tone (8,600,speed);
  delay (speed);
}

char lookup(char currentMark)
{
    currentDashJump = floor(currentDashJump / 2.0f);
    if (currentMark == '.')
    {                
        currentDecoderIndex++;
    }
    else if (currentMark == '-')
    {
        currentDecoderIndex += currentDashJump;
    }
    else if (currentMark == '\0')
    {
        currentDecoderIndex = 0;
        currentDashJump = 128;
        return '\0';
    }
    Serial.println(currentDecoderIndex);
    if (currentDecoderIndex == 73) return 3; // Display BK prosign
    else if (currentDecoderIndex == 90) return 4; // Display KN prosign
    else if (currentDecoderIndex == 14) return 5; // Display SK prosign    
    return lookupString[currentDecoderIndex];
}
