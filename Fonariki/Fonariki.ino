#include <SPI.h>
#include <EEPROM.h>

#define PWM0 6
#define PWM1 5
#define P0 2
#define P1 3
#define DRL0 4
#define DRL1 7

#define TURN A0
#define DRL A1
#define PL A2
#define STROBE 8
#define SHOW 9


byte F[4] = {P0, P1, DRL0, DRL1};

byte StepTurn = 0;
byte TurnIndex = 0;
unsigned long TurnTime;
byte TurnDelay;

byte StepAuto = 0;
unsigned long AutoTime;

byte StepDRL = 0;
byte DRLPWM = 255;
byte RealPWM = 255;
unsigned long DRLTime;

byte StepPL = 0;
unsigned long PWMTime = 0;

byte TURN_0[80] = {
  0b00000000, 0b00000001,
  0b00000000, 0b00000011,
  0b00000000, 0b00000111,
  0b00000000, 0b00001111,
  0b00000000, 0b00011111,
  0b00000000, 0b00111111,
  0b00000000, 0b01111111,
  0b00000000, 0b11111111,
  0b00000001, 0b11111111,
  0b00000011, 0b11111111,
  0b00000111, 0b11111111,
  0b00001111, 0b11111111,
  0b00011111, 0b11111111,
  0b00111111, 0b11111111,
  0b01111111, 0b11111111,
  0b11111111, 0b11111111,

  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,
  0b11111111, 0b11111111,

  0b11111111, 0b11111110,
  0b11111111, 0b11111100,
  0b11111111, 0b11111000,
  0b11111111, 0b11110000,
  0b11111111, 0b11100000,
  0b11111111, 0b11000000,
  0b11111111, 0b10000000,
  0b11111111, 0b00000000,
  0b11111110, 0b00000000,
  0b11111100, 0b00000000,
  0b11111000, 0b00000000,
  0b11110000, 0b00000000,
  0b11100000, 0b00000000,
  0b11000000, 0b00000000,
  0b10000000, 0b00000000,
  0b00000000, 0b00000000
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  //  SPI.setClockDivider(SPI_CLOCK_DIV128);

  pinMode(PWM0, OUTPUT);
  digitalWrite(PWM0, HIGH);
  pinMode(PWM1, OUTPUT);
  digitalWrite(PWM1, HIGH);

  pinMode(P0, OUTPUT);
  digitalWrite(P0, LOW);
  pinMode(P1, OUTPUT);
  digitalWrite(P1, LOW);
  pinMode(DRL0, OUTPUT);
  digitalWrite(DRL0, LOW);
  pinMode(DRL1, OUTPUT);
  digitalWrite(DRL0, LOW);

  SPI.transfer(0);

  digitalWrite(P0, HIGH);
  digitalWrite(P1, HIGH);
  digitalWrite(DRL0, HIGH);
  digitalWrite(DRL1, HIGH);
  digitalWrite(PWM0, LOW);
  digitalWrite(PWM1, LOW);

  TurnDelay = EEPROM.read(0);
  if ( TurnDelay == 255 ) TurnDelay = 20;
  DRLTime = millis() + 100;
}

void loop() {
  TaskTurn();
  TaskAuto();
  TaskDRL();
  TaskPWM();
}

void SendLED( byte A, byte D) {
  digitalWrite(A, LOW);
  SPI.transfer(D);
  digitalWrite(A, HIGH);
}

void TaskTurn() {
  switch (StepTurn) {
    case 0:
      if (digitalRead(TURN) == LOW) {
        StepTurn = 10;
        TurnIndex = 0;
        TurnTime = millis();
      }
      break;

    case 10:
      if (TurnTime <= millis()) {
        if (TurnIndex < 40) {
          TurnTime = millis() + TurnDelay;
          SendLED(P0, TURN_0[TurnIndex * 2 + 1]);
          SendLED(P1, TURN_0[TurnIndex * 2]);
          TurnIndex++;
        }
        else {
          StepTurn = 20;
        }
      }
      break;
    case 20:
      if (digitalRead(TURN) == HIGH) {
        StepTurn = 0;
      }
      break;
  }
}

void TaskAuto() {
  switch (StepAuto) {
    case 0:
      if (StepTurn != 0) {
        AutoTime = millis();
        StepAuto = 10;
      }
      break;
    case 10:
      if ( (digitalRead(TURN) == HIGH) && ((AutoTime + 200) < millis())) {
        StepAuto = 20;
      }
      break;
    case 20:
      if (digitalRead(TURN) == LOW) {
        TurnDelay += (((millis() - AutoTime) >> 6 ) - TurnDelay) >> 1;
        AutoTime = millis();
        StepAuto = 10;
      }
      else if ((AutoTime + 2000) < millis() ) {
        StepAuto = 30;
      }
      break;
    case 30:
      if (TurnDelay != (EEPROM.read(0))) EEPROM.write(0, TurnDelay);
      StepAuto = 0;
      break;
  }
}

void TaskDRL() {
  if (StepAuto != 0 ) {
    digitalWrite(PWM1, HIGH);
    DRLPWM = 255;
    RealPWM = 255;
  }
  else {
    if (digitalRead(PL) == LOW) DRLPWM = 223;
    else {
      if (digitalRead(DRL) == LOW) DRLPWM = 0;
      else DRLPWM = 255;
    }
  }

  /*  switch (StepDRL) {
      case 0:
        if ((digitalRead(DRL) == LOW) && (StepAuto == 0) ) {
          //        DRLPWM = 250;
          //        digitalWrite(PWM1, HIGH);
          SendLED(DRL0, 0xFF);
          SendLED(DRL1, 0xFF);
          StepDRL = 20;
          //        DRLTime = millis() + 500;
        }
        break;
      case 10:
        //      if (DRLPWM > 0) {
        //        if ( DRLTime < millis()) {
        //          analogWrite(PWM1, DRLPWM);
        //          DRLTime = millis() + 10;
        //          DRLPWM--;
        //        }
        //      }
        //      else {
        //        StepDRL = 20;
        //      }
        break;

      case 20:
        if ((digitalRead(DRL) == HIGH) || (StepAuto != 0) ) {
          SendLED(DRL0, 0x00);
          SendLED(DRL1, 0x00);
          StepDRL = 0;
        }
        break;
    }
  */
}

void TaskPWM() {
  if ( ( RealPWM != DRLPWM ) && ( PWMTime < millis()) ) {
    if (RealPWM > DRLPWM) RealPWM--;
    else RealPWM++;

    if (RealPWM == 255) {
      digitalWrite(PWM1, HIGH);
      SendLED(DRL0, 0x00);
      SendLED(DRL1, 0x00);
    }
    else {
      if (RealPWM == 0 ) {
        digitalWrite(PWM1, LOW);
        SendLED(DRL0, 0xFF);
        SendLED(DRL1, 0xFF);
      }
      else {
        analogWrite(PWM1, RealPWM);
        SendLED(DRL0, 0xFF);
        SendLED(DRL1, 0xFF);
      }
    }
    PWMTime = millis() + 1;
  }
}


