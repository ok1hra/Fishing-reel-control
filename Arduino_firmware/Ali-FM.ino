#include <EEPROM.h>
/*
avrdude -P usb -b 19200 -c usbasp -p atmega328p -U flash:w:/home/dan/Arduino/hra/ok1hra/Ali-FM/Ali-FM.ino.with_bootloader.eightanaloginputs.hex -U efuse:w:0xFD:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m


winch diameter 20cm = 0,628 m perimeter
*/

//===================================================
const float CountRawLong                  = 0.628;  // meter
int SecondAfterLastChangeStorageToEeprom  = 20;    // second
//===================================================

// #define DEBUG_ON_IPSW
float BattLevel[3]={12.2, 12.4, 12.7};  // 50% 75% 100%
byte BattLevelResult[3]={0,0,0};
bool BattLevelResultZeroFlash;
float ArefVoltage = 4.427;            // Measure on Aref pin 20 for calibrate
float VoltageDrop = 1.1;              // drop on diode and fuse
float Divider = 6;                    // voltage divider
int CounterRaw = 0;
float Counter = 0;
int CounterForEeprom = 0;
float CounterPrev = 0;
float Speed = 0;
long ButtReactTime[2]{0,200};
long SpeedTime[2]{0,1000};
long SlowRefreshTime[2]{0,1000};
long EepromTime[2]{0,SecondAfterLastChangeStorageToEeprom*1000};
bool EepromNeedChange=false;

const int CS  = 13;  //Pin 2 as chip selection output
const int WR  = 12;  //Pin 3 as read clock output
const int DATA= 7;  //Pin 4 as Serial data output
const int BACK_LIGHT = 5;
const int VoltagePin = A6;
const int PlusButtonPin = 8;
const int MinusButtonPin = 9;
const int StopButtonPin = 4;
const int LIGHT = A7;
#if defined(DEBUG_ON_IPSW)
  const int EncPinA = 3;
  const int EncPinB = A0;
#else
  const int EncPinA = 2;
  const int EncPinB = 3;
#endif

#define CS1    digitalWrite(CS, HIGH)
#define CS0    digitalWrite(CS, LOW)
#define WR1    digitalWrite(WR, HIGH)
#define WR0    digitalWrite(WR, LOW)
#define DATA1  digitalWrite(DATA, HIGH)
#define DATA0  digitalWrite(DATA, LOW)

#define sbi(x, y)  (x |= (1 << y))
#define cbi(x, y)  (x &= ~(1 <<y ))
#define uchar   unsigned char
#define uint   unsigned int

#define  ComMode    0x52
#define  RCosc      0x30
#define  LCD_on     0x06
#define  LCD_off    0x04
#define  Sys_en     0x02
#define  CTRl_cmd   0x80
#define  Data_cmd   0xa0

/*0,1,2,3,4,5,6,7,8,9,A,b,C,c,d,E,F,H,h,L,n,N,o,P,r,t,U,- (26), (27),degree (28),comma */
const char num[]={0x7D,0x60,0x3E,0x7A,0x63,0x5B,0x5F,0x70,0x7F,0x7B,0x77,0x4F,0x1D,0x0E,0x6E,0x1F,0x17,0x67,0x47,0x0D,0x46,0x75,0x37,0x06,0x0F,0x6D,0x02,0x00,0x33,0x80};
//char dispnum[6]={0x00,0x00,0x00,0x00,0x00,0x00};

int Digit[6];
bool LcdNeedRefresh = false;
long LcdRefresh[2]{0,500};

int ora, zora, uora;
int minut, zmin, umin;
int temperatura, temp, ztemp, utemp, stemp;
byte semn = 0;

//--------------------------------------------------------------------------------------------------------------
void setup() {
  #if defined(DEBUG_ON_IPSW)
    Serial.begin(115200);
  #endif
  analogReference(INTERNAL);
  Serial.begin(115200);
  Serial.println(" ");
  pinMode(CS, OUTPUT); //
  pinMode(WR, OUTPUT); //
  pinMode(DATA, OUTPUT); //
  pinMode(LIGHT, INPUT); //
  pinMode(BACK_LIGHT, OUTPUT); //
  analogWrite(BACK_LIGHT, 255);
  pinMode(EncPinA, INPUT_PULLUP); //
  pinMode(EncPinB, INPUT_PULLUP); //
  CS1;
  DATA1;
  WR1;
  delay(50);
  Init_1621();
  HT1621_all_on(16);
  // delay(500);
  // HT1621_all_off(16);
  LcdNeedRefresh=true;
  LcdShowAll((float(analogRead(VoltagePin)) * ArefVoltage * Divider / 1023.0 + VoltageDrop)*100);
  delay(2000);
  Counter=EEPROM.read(0)*100+EEPROM.read(1)*10+EEPROM.read(2);
  CounterForEeprom=int(Counter);
  CounterPrev=Counter;
  attachInterrupt(digitalPinToInterrupt(EncPinA), EncoderCount, FALLING);
  // detachInterrupt(digitalPinToInterrupt(EncPinA);
}
//--------------------------------------------------------------------------------------------------------------

void loop() {
  #if defined(DEBUG_ON_IPSW)
    // nil
  #else
  CounterSpeed();
  Buttons();


  if(millis()-ButtReactTime[0]<ButtReactTime[1]){
    LcdShow(int(Counter), 0);
  }else{
    LcdShow(int(Counter), abs(Speed));
  }

  float voltage = float(analogRead(VoltagePin)) * ArefVoltage * Divider / 1023.0 + VoltageDrop;
  if(millis()-SlowRefreshTime[0]>SlowRefreshTime[1]){
    Battery(voltage);

    // BackLight
    analogWrite(BACK_LIGHT, map(analogRead(LIGHT), 0, 1024, 255, 0) );

    // EEPROM
    if(CounterForEeprom!=int(Counter)){
      CounterForEeprom=int(Counter);
      EepromTime[0]=millis();
      EepromNeedChange=true;
    }
    if(millis()-EepromTime[0]>EepromTime[1] && EepromNeedChange==true){
      EepromNeedChange=false;
      EEPROM.write(0, Digit[0]);
      EEPROM.write(1, Digit[1]);
      EEPROM.write(2, Digit[2]);
      // analogWrite(BACK_LIGHT, 0);
    }
    SlowRefreshTime[0]=millis();
  }
  #endif
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void EncoderCount(){    // interrupt
  // detachInterrupt(digitalPinToInterrupt(EncPinA));
  if(digitalRead(EncPinB)==0){
    CounterRaw++;
  }else{
    CounterRaw--;
  }

  if(CounterRaw>999){
    CounterRaw=999;
  }
  if(CounterRaw<0){
    CounterRaw=0;
  }
  Counter = CounterRaw*CountRawLong;

  #if defined(DEBUG_ON_IPSW)
    for (int i=0; i <= 20; i++){
    Counter=0.5*i;
    CounterRaw=Counter*CountRawLong;
      Serial.print("CounterRaw ");
      Serial.print(CounterRaw);
      Serial.print(" = CountRawLong ");
      Serial.print(CountRawLong);
      Serial.print(" * Counter ");
      Serial.println(Counter);
    }
  #endif

  LcdNeedRefresh=true;
  // attachInterrupt(digitalPinToInterrupt(EncPinA), EncoderCount, FALLING);
}

//---------------------------------------------------------------------------------------------------------
void Battery(float voltage){

  if(voltage>BattLevel[2]){
    // 100%
    BattLevelResult[2]=0x80;
    BattLevelResult[1]=0x80;
    BattLevelResult[0]=0x80;
  }else if(voltage>BattLevel[1] && voltage<=BattLevel[2]){
    // 75%
    BattLevelResult[2]=0x00;
    BattLevelResult[1]=0x80;
    BattLevelResult[0]=0x80;
  }else if(voltage>BattLevel[0] && voltage<=BattLevel[1]){
    // 50%
    BattLevelResult[2]=0x00;
    BattLevelResult[1]=0x00;
    BattLevelResult[0]=0x80;
  }else if(voltage<=BattLevel[0]){
    // <50%
    BattLevelResult[2]=0x00;
    BattLevelResult[1]=0x00;
    BattLevelResult[0]=0x00;
  }
}

//---------------------------------------------------------------------------------------------------------

void CounterSpeed(){
  if(millis()-SpeedTime[0]>SpeedTime[1]){
    Speed=Counter-CounterPrev;
    CounterPrev=Counter;
    if(Speed>99){
      Speed=99;
    }
    if(Speed<-99){
      Speed=-99;
    }
    SpeedTime[0]=millis();
  }
}
//---------------------------------------------------------------------------------------------------------
void Buttons(){
  if(millis()-ButtReactTime[0]>ButtReactTime[1]){
    byte Plus = digitalRead(PlusButtonPin);
    byte Minus = digitalRead(MinusButtonPin);
    byte Stop = digitalRead(StopButtonPin);

    if(Plus==1 && Minus==0 && Stop==0){
      if(abs(Speed)>=3){
        Counter+=10;
      }else{
        Counter++;
      }
      if(Counter>999){Counter=999;}
      ButtReactTime[0]=millis();
      LcdNeedRefresh=true;
    }
    if(Plus==0 && Minus==1 && Stop==0){
      if(abs(Speed)>=3){
        Counter-=10;
      }else{
        Counter--;
      }
      if(Counter<0){Counter=0;}
      ButtReactTime[0]=millis();
      LcdNeedRefresh=true;
    }
    if(Plus==0 && Minus==0 && Stop==1){
      Counter=0;
      LcdNeedRefresh=true;
    }
    if(Plus==1 || Minus==1 || Stop==1){
      CounterRaw=Counter/CountRawLong;
    }
    Plus=0;
    Minus=0;
    Stop=0;
    ButtReactTime[0]=millis();
  }
}
//---------------------------------------------------------------------------------------------------------

void LcdShow(int LeftNumberToShow, int RightNumberToShow){
  if(millis()-LcdRefresh[0]>LcdRefresh[1] || LcdNeedRefresh == true){
    // split
    SplitNumberToDigit(LeftNumberToShow, 1);
    SplitNumberToDigit(RightNumberToShow, 0);
    HT1621_all_off(16);

    // RIGHT
    Write_1621(0,num[Digit[5]]);

    if(Digit[4]==0 && Digit[3]==0){
      if(Speed<0 && millis()-ButtReactTime[0]>ButtReactTime[1]){
        Write_1621(2,num[26]); // Negative sign
      }else{
        Write_1621(2,0x00);
      }
    }else{
      Write_1621(2,num[Digit[4]]);
    }

    if(Digit[3]==0){
      if(Speed<0 && Digit[4]!=0 && millis()-ButtReactTime[0]>ButtReactTime[1]){
        Write_1621(4,num[26]+num[29]); // Negative sign
      }else{
        Write_1621(4,0x00+num[29]);   // blank + dot
      }
    }else{
      Write_1621(4,num[Digit[3]]+num[29]);
    }


    // LEFT
    byte FLASH=0x00;
    if(BattLevelResult[0]==0){
      BattLevelResultZeroFlash= !BattLevelResultZeroFlash;
    }
    if(BattLevelResultZeroFlash==true){
      FLASH=0x80;
    }

    Write_1621(6,num[Digit[2]]+BattLevelResult[0]+FLASH);

    if(Digit[1]==0 && Digit[0]==0){
      Write_1621(8,0x00+BattLevelResult[1]);
    }else{
      Write_1621(8,num[Digit[1]]+BattLevelResult[1]);
    }

    if(Digit[0]==0){
      Write_1621(10,0x00+BattLevelResult[2]);
    }else{
      Write_1621(10,num[Digit[0]]+BattLevelResult[2]);
    }

  // Write_1621(2,num[Digit[4]]);
  // Write_1621(4,num[Digit[3]]+num[29]);  // number with decimal points
  // Write_1621(6,num[Digit[2]]);
  // Write_1621(8,num[Digit[1]]);
  // Write_1621(10,num[Digit[0]]);
  LcdRefresh[0]=millis();
  LcdNeedRefresh = false;
  }
}
//---------------------------------------------------------------------------------------------------------

void LcdShowAll(long NumberToShow){
  if(millis()-LcdRefresh[0]>LcdRefresh[1] || LcdNeedRefresh == true){
    // split
    SplitNumberToAllDigit(NumberToShow);
    HT1621_all_off(16);

    Write_1621(0,num[Digit[5]]);

    Write_1621(2,num[Digit[4]]+num[29]);

    Write_1621(4,num[Digit[3]]);


    byte FLASH=0x00;
    if(BattLevelResult[0]==0){
      BattLevelResultZeroFlash= !BattLevelResultZeroFlash;
    }
    if(BattLevelResultZeroFlash==true){
      FLASH=0x80;
    }

    if(Digit[2]==0 && Digit[1]==0 && Digit[0]==0){
      Write_1621(6,0x00+BattLevelResult[0]+FLASH);
    }else{
      Write_1621(6,num[Digit[2]]+BattLevelResult[0]+FLASH);
    }

    if(Digit[1]==0 && Digit[0]==0){
      Write_1621(8,0x00+BattLevelResult[1]);
    }else{
      Write_1621(8,num[Digit[1]]+BattLevelResult[2]);
    }

    if(Digit[0]==0){
      Write_1621(10,0x00+BattLevelResult[2]);
    }else{
      Write_1621(10,num[Digit[0]]+BattLevelResult[2]);
    }

  // Write_1621(2,num[Digit[4]]);
  // Write_1621(4,num[Digit[3]]+num[29]);  // number with decimal points
  // Write_1621(6,num[Digit[2]]);
  // Write_1621(8,num[Digit[1]]);
  // Write_1621(10,num[Digit[0]]);
  LcdRefresh[0]=millis();
  LcdNeedRefresh = false;
  }
}
//---------------------------------------------------------------------------------------------------------

void SplitNumberToDigit(int NUMBER, bool LEFT){
  if(LEFT==false){
    Digit[5] = (NUMBER / 1) % 10;
    Digit[4] = (NUMBER / 10) % 10;
    Digit[3] = (NUMBER / 100) % 10;
    // Digit[2] = (NUMBER / 1000) % 10;
    // Digit[1] = (NUMBER / 10000) % 10;
    // Digit[0] = (NUMBER / 100000) % 10;
  }else{
    Digit[2] = (NUMBER / 1) % 10;
    Digit[1] = (NUMBER / 10) % 10;
    Digit[0] = (NUMBER / 100) % 10;
  }
}
//---------------------------------------------------------------------------------------------------------

void SplitNumberToAllDigit(long NUMBER){
    Digit[5] = (NUMBER / 1) % 10;
    Digit[4] = (NUMBER / 10) % 10;
    Digit[3] = (NUMBER / 100) % 10;
    Digit[2] = (NUMBER / 1000) % 10;
    Digit[1] = (NUMBER / 10000) % 10;
    Digit[0] = (NUMBER / 100000) % 10;
}
//---------------------------------------------------------------------------------------------------------
// http://arduinolearning.com/code/ht1621-6-digit-7-segment-display-example.php
// random clock and temperature with decimal point by Nicu FLORICA (niq_ro) - http://www.arduinotehniq.com
// ver.4 - Craiova, 14.10.2018

void DEMO(){
  ora = random(24);
  zora = ora/10;
  uora = ora%10;
Serial.print(ora);
Serial.print(" = ");
Serial.print(zora);
Serial.print(uora);
Serial.println(" ?");
  minut = random(60);
  zmin = minut/10;
  umin = minut%10;
Serial.print(minut);
Serial.print(" = ");
Serial.print(zmin);
Serial.print(umin);
Serial.println(" ?");

  temperatura = random(-390,390);
  if (temperatura < 0)
  {
    temp = - temperatura;
    semn = 2 ;  // negative
  }
else
   {
   temp = temperatura;
   semn = 0 ;  // negative
   }

   ztemp = temp/100;
   temp = temp%100;
   utemp = temp/10;
   stemp = temp%10;
Serial.print(temperatura);
Serial.print(" = ");
if (semn ==2) Serial.print("-");
else Serial.print("+");
Serial.print(ztemp);
Serial.print(utemp);
Serial.print(",");
Serial.print(stemp);
Serial.println(" ?");

HT1621_all_off(16);

  Write_1621(8,num[zora]);
  Write_1621(6,num[uora]);
  Write_1621(2,num[zmin]);
  Write_1621(0,num[umin]);

for (int i=0; i <= 3; i++)
{
  Write_1621(4,num[26]);  // -
  delay(1000);
  Write_1621(4,num[27]);  // empty space
  delay(1000);
}

   HT1621_all_off(16);
   delay(1000);

if (ztemp != 0)
{
if (semn == 2) Write_1621(10,num[26]); // -
  Write_1621(8,num[ztemp]);
}
else
if (semn == 2) Write_1621(8,num[26]); // -
  Write_1621(6,num[utemp]);
  Write_1621(4,num[stemp]+num[29]);  // number with decimal points
  Write_1621(2,num[28]);  // degree
  Write_1621(0,num[12]);  // C
  delay(3000);

}



void SendBit_1621(uchar sdata,uchar cnt)
{
  uchar i;
  for(i=0;i<cnt;i++)
  {
    WR0;
    if(sdata&0x80) DATA1;
    else DATA0;
    WR1;
    sdata<<=1;
  }
}

void SendCmd_1621(uchar command)
{
  CS0;
  SendBit_1621(0x80,4);
  SendBit_1621(command,8);
  CS1;
}

void Write_1621(uchar addr,uchar sdata)
{
  addr<<=2;
  CS0;
  SendBit_1621(0xa0,3);
  SendBit_1621(addr,6);
  SendBit_1621(sdata,8);
  CS1;
}

void HT1621_all_off(uchar num)
{
  uchar i;
  uchar addr=0;
  for(i=0;i<num;i++)
  {
    Write_1621(addr,0x00);
    addr+=2;
  }
}

void HT1621_all_on(uchar num)
{
  uchar i;
  uchar addr=0;
  for(i=0;i<num;i++)
  {
    Write_1621(addr,0xff);
    addr+=2;
  }
}

void Init_1621(void)
{
  SendCmd_1621(Sys_en);
  SendCmd_1621(RCosc);
  SendCmd_1621(ComMode);
  SendCmd_1621(LCD_on);
}
