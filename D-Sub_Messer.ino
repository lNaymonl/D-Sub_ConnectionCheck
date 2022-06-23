#include <LiquidCrystal.h>

// [0] digitalPins  (write) side a
// [1] analogPins   (read)  side b
const int dSub_pins[2][9];

int connected_dSub_pin[9];
float voltage_dSub_pin[9];

// Button pin to restart
const int rsBtn;

bool finished = false;
bool printed = false;

// Check if button (pullup) is pressed
const bool pressed(int buttonPin) {
  if (!digitalRead(buttonPin)) {
    return true;
  } else {
    return false;
  }
}

// LCD-Dislpay Pins
const int rs = 11, en = 15, d4 = 19, d5 = 23, d6 = 27, d7 = 31;

// LCD definition
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Keywords
String analog = "analog", digital = "digital", pwm = "pwm", volt = "volt";

// Function to read a pin
// type = analog/digital/pwm
// output = byte/voltage
const float read(String type, int pin, String output = "byte") {
  if (type == analog) {
    if (output == "byte") {
      return analogRead(pin);
    } else if (output == "volt") {
      return 5.0 / 1023.0 * analogRead(pin);
    }
  } else if (type == digital) {
    return digitalRead(pin);
  } else if (type == pwm) {
    if (output == "byte") {
      return pulseIn(pin, HIGH);
    } else if (output == "volt") {
      return 5.0 / 1023.0 * pulseIn(pin, HIGH);
    }
  }
}

// Function to set pin Voltage/State
// type = analog/digital
// value = byte/LOW/HIGH
void write(String type, int pin, byte value) {
  if (type == analog) {
    analogWrite(pin, value);
  } else if (type = digital) {
    digitalWrite(pin, value);
  }
}

// pinMode shortcut
void pM(uint8_t pin, uint8_t mode) { pinMode(pin, mode); }

// Delay in seconds
void d(float durationInSec) { delay(durationInSec * 1000); }

// Get wich pin is connected on side b
void getConnectedPin(int pin) {
  write(digital, pin, HIGH);
  d(0.1);

  for (int i = 0; i < 9; i++) {
    if (read(analog, i, volt) <= 1) {
      connected_dSub_pin[i] = i;
      for (int x = 1; x <= 3; x++) {
        voltage_dSub_pin[i] += read(analog, i, volt);
        d(0.25);
      }
      voltage_dSub_pin[i] /= 3;
      break;
    }
  }
  write(digital, pin, LOW);
}

// Write numbers to lcd
void writeLcd(int cursorX, int cursorY, char content) {
  lcd.setCursor(cursorX, cursorY);
  lcd.write(content);
}

// Write strings to lcd
void writeLcd(int cursorX, int cursorY, char *content) {
  lcd.setCursor(cursorX, cursorY);
  lcd.write(content);
}

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);

  for (int i = 0; i < 9; i++) {
    pM(dSub_pins[0][i], OUTPUT);
  }
  pM(rsBtn, INPUT_PULLUP);

  for (int i = 0; i < 9; i++) {
    connected_dSub_pin[i] = 0;
    voltage_dSub_pin[i] = 0;
    writeLcd(i, 0, (i + 1));
  }
}

void loop() {
  if (finished == false) {
    for (int i = 0; i < 9; i++) {
      getConnectedPin(i);
    }
    finished = true;
    printed = false;
  } else if (printed == false) {
    int avrVolt;
    for (int i = 0; i < 9; i++) {
      avrVolt += voltage_dSub_pin[i];
    }
    avrVolt /= 9;
    for (int i = 0; i < 9; i++) {
      writeLcd(i, 1, connected_dSub_pin[i]);
      writeLcd(10, 0, avrVolt);
      writeLcd(13, 0, "V");
    }
    printed = true;
  }

  if (pressed(rsBtn)) {
    finished = false;
    for (int i = 0; i < 9; i++) {
      connected_dSub_pin[i] = 0;
      voltage_dSub_pin[i] = 0;
    }
  }
}