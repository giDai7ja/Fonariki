// Fuses OK (E:FD, H:DA, L:FF)

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

#define StrobH 10
#define StrobL 90
#define StrobP 1000

byte F[4] = {P0, P1, DRL0, DRL1};

byte TurnDelay;
byte StepStrobe = 0;
byte StepShow = 0;
byte StepPWM = 0;

unsigned long DRLTime = 0;
unsigned long TimeStrobe = 0;

//double tmp;

// ----- Инициализация
void setup() {
  Serial.begin(19200);
  SPI.begin();
  //  SPI.setClockDivider(SPI_CLOCK_DIV128);

  digitalWrite(PWM0, HIGH);
  pinMode(PWM0, OUTPUT);
  digitalWrite(PWM1, HIGH);
  pinMode(PWM1, OUTPUT);

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

  SPI.transfer(0xFF);

  digitalWrite(DRL0, HIGH);
  digitalWrite(DRL1, HIGH);

  digitalWrite(PWM0, LOW);

  TurnDelay = EEPROM.read(0);
  if ( TurnDelay == 255 ) TurnDelay = 20;
  DRLTime = millis() + 200;
}
// ----- Конец инициализации



// ----- Главный цикл
void loop() {
  TaskTurn();
  TaskAuto();
  TaskDRL();
  TaskPWM();
  TaskEffects();
}
// ----- Конец главного цикла


// Подпрограмма записи в регистр
void SendLED( byte A, byte D) {
  digitalWrite(A, LOW);
  SPI.transfer(D);
  digitalWrite(A, HIGH);
}
// ----- Конец записи



// ---------- Эффект поворотника
const byte TURN_0[96] PROGMEM = {
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
byte StepTurn = 0;
byte TurnIndex = 0;
unsigned long TurnTime;
void TaskTurn() {
  switch (StepTurn) {
    case 0:
      if (digitalRead(TURN) == LOW) {
        TurnTime = millis();
        TurnIndex = 0;
        if ( (StepStrobe != 0) || (StepShow != 0 ) ) OutputReset();
        StepTurn = 10;
      }
      break;

    case 10:
      if (TurnTime <= millis()) {
        if (TurnIndex < 48) {
          TurnTime += TurnDelay;
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
// ------------- Конец эффекта поворотника



// -------------- Автонастройка скорости эффекта поворотов
byte StepAuto = 0;
unsigned long AutoTime;
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
      else if ((AutoTime + 3000) < millis() ) StepAuto = 30;
      break;
    case 20:
      if (digitalRead(TURN) == LOW) {
        TurnDelay += (((millis() - AutoTime) / 64 ) - TurnDelay) / 2;
        //        TurnDelay += (((millis() - AutoTime) / 64) - TurnDelay) >> 1;
        AutoTime = millis();
        StepAuto = 10;
        //        Serial.println(TurnDelay);
      }
      else if ((AutoTime + 3000) < millis() ) {
        StepAuto = 30;
      }
      break;
    case 30:
      if (TurnDelay != (EEPROM.read(0))) EEPROM.write(0, TurnDelay);
      StepAuto = 0;
      break;
  }
}
// -------- Конец автоподстройки



// -------- Дневные ходовые огни + габариты
byte StepDRL = 0;
byte DRLPWM = 255;
byte RealPWM = 255;
void TaskDRL() {
  if (StepAuto != 0 ) {
    digitalWrite(PWM1, HIGH);
    DRLPWM = 255;
    RealPWM = 255;
    StepPWM = 0;
  }
  else {
    if (digitalRead(PL) == LOW) DRLPWM = 223;
    else {
      if (digitalRead(DRL) == LOW) DRLPWM = 0;
      else DRLPWM = 255;
    }
  }
}
// ------- Конец ДХО и габаритов



// -------- Изменения параметров ШИМ
unsigned long PWMTime = 0;
void TaskPWM() {
  switch (StepPWM) {
    case 0: if ( RealPWM != DRLPWM ) {
        PWMTime = millis();
        StepPWM = 10;
      }
      break;

    case 10:
      if ( PWMTime < millis()) {
        if ( RealPWM == DRLPWM ) StepPWM = 0;
        else {
          if (RealPWM > DRLPWM) RealPWM--;
          else RealPWM++;

          switch (RealPWM) {
            case 255: digitalWrite(PWM1, HIGH);
              break;

            case 0: digitalWrite(PWM1, LOW);
              break;

            default:
              analogWrite(PWM1, RealPWM );
          }
          PWMTime += 2;
          //          Serial.println(RealPWM);
        }
      }
      break;
  }
}
// ------ Конец изменения ШИМ



// ------ Эффекты
byte StepEff = 0;
void TaskEffects() {
  // Еффекты. Работают только если остальное не работает
  //  if ((StepAuto == 0) && (digitalRead(PL) == HIGH) && (digitalRead(DRL) == HIGH)) {
  if ((StepAuto == 0) && (RealPWM == 255)) {
    if (digitalRead(STROBE) == LOW || StepStrobe != 0 ) TaskStrobe();
    else {
      if (digitalRead(SHOW) == LOW ) {
        TaskShow();
      }
    }
  }
  else {
    // Возвращаем всё как было
    StepStrobe = 0;
    TimeStrobe = 0;
    StepShow = 0;
  }
}
// ------ Конец эффектов



// ------ Стробоскоп
void TaskStrobe() {
  if (TimeStrobe < millis()) {
    switch (StepStrobe) {
      case 0:
        TimeStrobe = millis();
        StepStrobe = 10;
        break;

      case 10:
        digitalWrite(PWM1, LOW);
        TimeStrobe += StrobH;
        StepStrobe = 20;
        break;

      case 20:
        digitalWrite(PWM1, HIGH);
        TimeStrobe += StrobL;
        StepStrobe = 30;
        break;

      case 30:
        digitalWrite(PWM1, LOW);
        TimeStrobe += StrobH;
        StepStrobe = 40;
        break;

      case 40:
        digitalWrite(PWM1, HIGH);
        TimeStrobe += StrobL;
        StepStrobe = 50;
        break;

      case 50:
        digitalWrite(PWM1, LOW);
        TimeStrobe += StrobH;
        StepStrobe = 60;
        break;

      case 60:
        digitalWrite(PWM1, HIGH);
        TimeStrobe += StrobP;
        StepStrobe = 70;
        break;

      case 70:
        StepStrobe = 0;
        break;

      default:
        StepStrobe = 0;
        break;
    }
  }
}
// ----- Конец стробоскопа



// ----- Демо режим
unsigned long ShowTime = 0;
void TaskShow() {

}
// ----- Конец демо



// ----- Сброс параметров
void OutputReset() {
  digitalWrite(PWM0, HIGH);
  digitalWrite(PWM1, HIGH);
  SendLED(P0, 0);
  SendLED(P1, 0);
  SendLED(DRL0, 0xFF);
  SendLED(DRL0, 0xFF);
  digitalWrite(PWM0, LOW);
}
// ----- Конец сброса
