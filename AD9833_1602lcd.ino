#include "rotary.h"
#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 7, 6, 5, 4, 3);
#include <SPI.h>
#define FSYNC         10
enum WaveType {
  SINE              = 0x2000,
  SQUARE            = 0x2028,
  TRIANGLE          = 0x2002
};
WaveType wave       = SINE;
String MenuItem     = "";
String freq_Step    = "";
byte waveType       = 0;
long Freq           = 100;
long OldFreqStep    = 100;
long FreqStep       = 1;
byte menu           = 2;
char waveItem       = 0;
char FS             = 0;
byte customCharSin1[] = {
  B00100,
  B01010,
  B10001,
  B10001,
  B00000,
  B00000,
  B00000,
  B00000
};
byte customCharSin2[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B10001,
  B10001,
  B01010,
  B00100
};
byte customCharTri1[] = {
  B00000,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000,
  B00000,
  B00000
};
byte customCharTri2[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B10001,
  B01010,
  B00100,
  B00000
};
byte customCharSqu1[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B00000
};
byte customCharSqu2[] = {
  B00000,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B00000
};
byte customCharSqu3[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B00000
};
void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(FSYNC, OUTPUT);
  digitalWrite(FSYNC, HIGH);
  SPI.begin();
  delay(50);
  AD9833reset();
  AD9833Frequency(Freq, wave);
  RotaryInit();
  lcd.begin(16, 2);
  lcd.createChar(0, customCharSin1);
  lcd.createChar(1, customCharSin2);
  lcd.createChar(2, customCharTri1);
  lcd.createChar(3, customCharTri2);
  lcd.createChar(4, customCharSqu1);
  lcd.createChar(5, customCharSqu2);
  lcd.createChar(6, customCharSqu3);
  lcd.setCursor(0, 0);
  lcd.print("    ARDUINO");
  lcd.setCursor(0, 1);
  lcd.print("GELSTRME PLTFRMU");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  AD9833  DDS  ");
  lcd.setCursor(0, 1);
  lcd.print("   GENERATOR   ");
  delay(2000);
}
void loop() {
  lcd.clear();
  while ( true ) {
    if ( RotaryGetStatusB() ) {
      delay(250);
      menu++;
      if ( menu >= 3 ) menu = 0;
      if ( menu == 0 ) RotaryResetStatus(waveItem);
      if ( menu == 1 ) RotaryResetStatus(FS);
      if ( menu == 2 ) {
        RotaryResetPosition();
        RotaryResetStatus(Freq);
      }
    }
    lcd.setCursor(0, 0);
    lcd.print(MenuItem);
    if ( waveItem == 0 ) {
      lcd.write(byte(0));
      lcd.write(byte(1));
      lcd.write(byte(0));
    }
    if ( waveItem == 1 ) {
      lcd.write(byte(2));
      lcd.write(byte(3));
      lcd.write(byte(2));
    }
    if ( waveItem == 2 ) {
      lcd.write(byte(4));
      lcd.write(byte(5));
      lcd.write(byte(6));
    }
    lcd.setCursor(5, 0);
    lcd.print(freq_Step);
    if ( FreqStep < OldFreqStep ) lcd.print("          ");
    lcd.setCursor(6, 0);
    lcd.print(FreqStep);
    //lcd.print("Step");
    lcd.setCursor(0, 1);
    lcd.print("FREQ: ");
    lcd.print(Freq);
    lcd.print("Hz         ");
    OldFreqStep = FreqStep;
    if ( menu == 0 ) {
      waveItem = RotaryGetStatus();
      if ( waveItem <  0 ) RotaryResetStatus(2);
      if ( waveItem >  2 ) RotaryResetStatus(0);
      if ( waveItem == 0 ) {
        MenuItem = ">";//">SIN";
        freq_Step = " ";
        wave = SINE;
        Update();
      } else if ( waveItem == 1 ) {
        MenuItem = ">";//">TRI";
        freq_Step = " ";
        wave = TRIANGLE;
        Update();
      } else if ( waveItem == 2 ) {
        MenuItem = ">";//">SQE";
        freq_Step = " ";
        wave = SQUARE;
        Update();
      }
    } else if ( menu == 1 ) {
      if ( waveItem == 0 ) MenuItem = " ";//" SIN";
      if ( waveItem == 1 ) MenuItem = " ";//" TRI";
      if ( waveItem == 2 ) MenuItem = " ";//" SQE";
      freq_Step = ">";
      FS = RotaryGetStatus();
      if ( FS < 1 ) RotaryResetStatus(FS = 1);
      if ( FS > 7 ) RotaryResetStatus(FS = 7);
      if ( FS == 1 ) FreqStep = 1;
      if ( FS == 2 ) FreqStep = 10;
      if ( FS == 3 ) FreqStep = 100;
      if ( FS == 4 ) FreqStep = 1000;
      if ( FS == 5 ) FreqStep = 10000;
      if ( FS == 6 ) FreqStep = 100000;
      if ( FS == 7 ) FreqStep = 1000000;
    } else if ( menu == 2 ) {
      if ( waveItem == 0 ) MenuItem = " ";//" SIN";
      if ( waveItem == 1 ) MenuItem = " ";//" TRI";
      if ( waveItem == 2 ) MenuItem = " ";//" SQE";
      freq_Step = " ";
      if ( RotaryLeftPosition() ) {
        RotaryResetPosition();
        Freq = Freq - FreqStep;
        if ( Freq < 1 ) Freq = 1;
        Update();
      }
      if ( RotaryRightPosition() ) {
        RotaryResetPosition();
        Freq = Freq + FreqStep;
        if ( Freq > 12500000 ) Freq = 12500000;
        Update();
      }
    }
  }
}
void AD9833reset() {
  WriteRegister(0x100);
  delay(10);
}
void Update() {
  AD9833Frequency(Freq, wave);
}
void AD9833Frequency(long frequency, int Waveform) {
  long FreqWord = (frequency * pow(2, 28)) / 25.0E6;
  int MSB = (int)((FreqWord & 0xFFFC000) >> 14);
  int LSB = (int)(FreqWord & 0x3FFF);
  LSB |= 0x4000;
  MSB |= 0x4000;
  WriteRegister(0x2100);
  WriteRegister(LSB);
  WriteRegister(MSB);
  WriteRegister(0xC000);
  WriteRegister(Waveform);
}
void WriteRegister(int Data) {
  SPI.setDataMode(SPI_MODE2);
  digitalWrite(FSYNC, LOW);
  delayMicroseconds(10);
  SPI.transfer(Data >> 8);
  SPI.transfer(Data & 0xFF);
  digitalWrite(FSYNC, HIGH);
  SPI.setDataMode(SPI_MODE0);
}
