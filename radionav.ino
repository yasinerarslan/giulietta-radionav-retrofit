#include <EEPROM.h>

#define POT_PIN A0

#define VOLTAGE_TRIGGER 13

#define BTN_OPEN 3
#define BTN_ANG_UP 4
#define BTN_ANG_DOWN 5

#define DRV_STBY 6
#define DRV_AIN1 7
#define DRV_AIN2 8
#define DRV_PWMA 9

// Global Params
int ON_SCR = 1;
int CLOSE_SCR = -1;
int PRESSED_BTN = 0;
int SENSETIVE = 20;
int CHANGE_ANGLE = 30;
int CLOSE_POT_POS = 850;
int MIDDLE_POT_POS = 400;
int OPEN_POT_POS = 60;
int SAVED_POT_POS = OPEN_POT_POS;
int LAST_TRIGGER_STATE = 0;

void setup() {
  pinMode(DRV_STBY, OUTPUT);
  pinMode(DRV_AIN1, OUTPUT);
  pinMode(DRV_AIN2, OUTPUT);
  pinMode(DRV_PWMA, OUTPUT);
  
  pinMode(VOLTAGE_TRIGGER, INPUT);
  pinMode(BTN_OPEN, INPUT_PULLUP);
  pinMode(BTN_ANG_UP, INPUT_PULLUP);
  pinMode(BTN_ANG_DOWN, INPUT_PULLUP);

  int EEPROM_TEMP = EEPROMLoad(0);

  if (EEPROM_TEMP <= MIDDLE_POT_POS) SAVED_POT_POS = EEPROM_TEMP;
  else EEPROMSave(0, OPEN_POT_POS);

  // if (digitalRead(VOLTAGE_TRIGGER)) moveToAngle(ON_SCR, SAVED_POT_POS);

  Serial.begin(9600);
}

void loop() {
  int TRIGGER_NOW = digitalRead(VOLTAGE_TRIGGER);

  if (TRIGGER_NOW == HIGH && LAST_TRIGGER_STATE == LOW) {
    moveToAngle(ON_SCR, SAVED_POT_POS);
  }

  if (TRIGGER_NOW == LOW && LAST_TRIGGER_STATE == HIGH) {
    moveToAngle(CLOSE_SCR, CLOSE_POT_POS);
  }

  LAST_TRIGGER_STATE = TRIGGER_NOW;

  if (!digitalRead(VOLTAGE_TRIGGER)) return;

  if (PRESSED_BTN != 0) {
    if (!digitalRead(PRESSED_BTN)) return;
    else PRESSED_BTN = 0;
  }

  if (!digitalRead(BTN_OPEN)) {
    PRESSED_BTN = BTN_OPEN;
    if (readAngle() > (SAVED_POT_POS + SENSETIVE)) moveToAngle(ON_SCR, SAVED_POT_POS);
    else moveToAngle(CLOSE_SCR, CLOSE_POT_POS);
    Serial.println("OPEN");
  }

  if (!digitalRead(BTN_ANG_UP)) {
    PRESSED_BTN = BTN_ANG_UP;
    int NEW_POS = readAngle() - CHANGE_ANGLE;
    if (NEW_POS < MIDDLE_POT_POS) {
      if (OPEN_POT_POS >= NEW_POS) moveToAngle(ON_SCR, OPEN_POT_POS);
      else {
        EEPROMSave(0, NEW_POS);
        SAVED_POT_POS = NEW_POS;
        moveToAngle(ON_SCR, NEW_POS);
      }
    }
    Serial.println("UP");
  }

  if (!digitalRead(BTN_ANG_DOWN)) {
    PRESSED_BTN = BTN_ANG_DOWN;
    int NEW_POS = readAngle() + CHANGE_ANGLE;
    if (NEW_POS < MIDDLE_POT_POS) {
      if (MIDDLE_POT_POS <= NEW_POS) moveToAngle(CLOSE_SCR, MIDDLE_POT_POS);
      else {
        EEPROMSave(0, NEW_POS);
        SAVED_POT_POS = NEW_POS;
        moveToAngle(CLOSE_SCR, NEW_POS);
      };
    }
    Serial.println("DOWN");
  }

  delay(200);
}

void moveToMotor(int direction) {
  digitalWrite(DRV_STBY, HIGH);
  if(direction == ON_SCR) {
    digitalWrite(DRV_AIN1, HIGH);
    digitalWrite(DRV_AIN2, LOW);
    analogWrite(DRV_PWMA, 50);
  } else if(direction == CLOSE_SCR) {
    digitalWrite(DRV_AIN1, LOW);
    digitalWrite(DRV_AIN2, HIGH);
    analogWrite(DRV_PWMA, 60);
  } else {
    digitalWrite(DRV_AIN1, LOW);
    digitalWrite(DRV_AIN2, LOW);
    digitalWrite(DRV_STBY, LOW);
    analogWrite(DRV_PWMA, 0);
  }
}

void moveToAngle(int direction, int targetPosition) {
  int position = readAngle();
  while(abs(position - targetPosition) > SENSETIVE) {
    position = readAngle();
    if (direction == ON_SCR) if (position > targetPosition) moveToMotor(-1);
    if (direction == CLOSE_SCR) if (targetPosition > position) moveToMotor(1);
  }
  moveToMotor(0);
}

int readAngle() {
  int POT_VAL = analogRead(POT_PIN);
  // Serial.print("POT VALUE > RAW =");
  // Serial.println(POT_VAL);
  return POT_VAL;
}

void EEPROMSave(int address, int value) {
  if (value < 0) value = 0;
  if (value > 1023) value = 1023;
  byte low  = value & 0xFF;
  byte high = (value >> 8) & 0x03;
  EEPROM.write(address, low);
  EEPROM.write(address + 1, high);
}

int EEPROMLoad(int address) {
  byte low  = EEPROM.read(address);
  byte high = EEPROM.read(address + 1);
  int value = (high << 8) | low;
  if (value < 0) value = 0;
  if (value > 1023) value = 1023;
  return value;
}