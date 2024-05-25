#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>

const char title[] = "Fan Level:";
const int tim = 250;
const int irReceiverPin = 7;

LiquidCrystal_I2C lcd(0x27, 16, 2);

IRrecv irrecv(irReceiverPin);
decode_results results;

int last_number = -1;

const int motorIn1 = 9;
const int motorIn2 = 10;

int get_number(unsigned long result) {
  Serial.print("result:");
  Serial.println(result);
if (result == 0xFF6897) { return 0; }
if (result == 0xFF30CF) { return 1; }
if (result == 0xFF18E7) { return 2; }
if (result == 0xFF7A85) { return 3; }
if (result == 0xFF10EF) { return 4; }
if (result == 0xFF38C7) { return 5; }
if (result == 0xFF5AA5) { return 6; }
if (result == 0xFF42BD) { return 7; }
if (result == 0xFF4AB5) { return 8; }
if (result == 0xFF52AD) { return 9; }
return -1;
}

void setup() {
  // lcd 
  lcd.init();
  lcd.backlight();

  // debug
  Serial.begin(9600);
  
  // irrecv
  irrecv.enableIRIn();

  // fan
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(title);
  if (irrecv.decode(&results)) {
    Serial.print("irCode: ");
    Serial.print(results.value, HEX);
    Serial.print(", bits: ");
    Serial.println(results.bits);
    irrecv.resume();
  }
  const int number = get_number(results.value);
  if (number != -1) { last_number = number; }
  if (last_number != -1) {
    int print_number = last_number;
    if (last_number > 5) {
      print_number = last_number - 5;
      lcd.setCursor(0, 0);
      lcd.print(title);
      for (int i = 0; i < 16 - strlen(title) - 5; i++) {
        lcd.print(' ');
      }
      for (int i = 0; i < 5; i++) {
        lcd.print('o');
      }
    }
    lcd.setCursor(0, 1);
    lcd.print(last_number);
    for (int i = 0; i < 15 - print_number; i++) {
      lcd.print(' ');
    }
    for (int i = 0; i < print_number; i++) {
      lcd.print('o');
    }
    analogWrite(motorIn1, 0);
    int fan_number = 30 * last_number;
    if (fan_number > 250) { fan_number = 250;}
    analogWrite(motorIn2, fan_number);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("-1");
  }
  delay(600);
}
