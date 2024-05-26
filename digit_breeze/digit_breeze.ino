#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>

// 定义LCD显示屏的地址和尺寸
const int kLcdAddr = 0x27;
const int kLcdCols = 16;
const int kLcdRows = 2;
const int kMaxOPerRow = 5;
const char kTitle[] = "Fan Level:";

// 定义红外接收器连接的引脚
const int kIrReceiverPin = 7;

// 定义电机控制引脚
const int kMotorIn1 = 9;
const int kMotorIn2 = 10;
const int kMaxFanLevel = 250;
const int kFanLevelFactor = 30;

// 定义红外码与数字的映射
enum class IrCodes {
  kIrCode0 = 0xFF6897,
  kIrCode1 = 0xFF30CF,
  kIrCode2 = 0xFF18E7,
  kIrCode3 = 0xFF7A85,
  kIrCode4 = 0xFF10EF,
  kIrCode5 = 0xFF38C7,
  kIrCode6 = 0xFF5AA5,
  kIrCode7 = 0xFF42BD,
  kIrCode8 = 0xFF4AB5,
  kIrCode9 = 0xFF52AD
};

const int kDelayTime = 250;

// 初始化LCD显示屏
LiquidCrystal_I2C g_lcd(kLcdAddr, kLcdCols, kLcdRows);

// 初始化红外接收器
IRrecv g_irrecv(kIrReceiverPin);

// 用于存储上一次接收到的数字
int g_fan_level = -1;

// 函数：根据红外信号的值返回对应的数字
int GetNumber(int value) {
  switch (value) {
    case kIrCode0: return 0;
    case kIrCode1: return 1;
    case kIrCode2: return 2;
    case kIrCode3: return 3;
    case kIrCode4: return 4;
    case kIrCode5: return 5;
    case kIrCode6: return 6;
    case kIrCode7: return 7;
    case kIrCode8: return 8;
    case kIrCode9: return 9;
    default: return -1;
  }
}

// setup函数：在程序开始时执行一次
void setup() {
  g_lcd.init();
  g_lcd.backlight();
  Serial.begin(9600);
  g_irrecv.enableIRIn();
  pinMode(kMotorIn1, OUTPUT);
  pinMode(kMotorIn2, OUTPUT);
}

// loop函数：在程序中不断循环执行
void loop() {
  g_lcd.clear();
  g_lcd.setCursor(0, 0);
  g_lcd.print(kTitle);
  decode_results results;
  if (g_irrecv.decode(&results)) {
    Serial.print("irCode: ");
    Serial.print(results.value, HEX);
    Serial.print(", bits: ");
    Serial.println(results.bits);
    g_irrecv.resume();
  }
  const int number = GetNumber(results.value);
  if (number != -1) {
    g_fan_level = number;
  }
  DisplayFanLevel();
  delay(kDelayTime);
}

void PrintSpaceAndO(int total_rest_number, int number_o) {
  for (int i = 0; i < total_rest_number - number_o; i++) {
    g_lcd.print(' ');
  }
  for (int i = 0; i < number_o; i++) {
    g_lcd.print('o');
  }
}

// 显示风扇级别
void DisplayFanLevel() {
  if (g_fan_level == -1) {
    g_lcd.setCursor(0, 1);
    g_lcd.print("-1");
    return;
  }
  int second_row_print_number = g_fan_level;
  // print first row with 'ooooo'
  if (g_fan_level > kMaxOPerRow) {
    second_row_print_number = g_fan_level - kMaxOPerRow;
    PrintSpaceAndO(kLcdCols - strlen(kTitle), kMaxOPerRow);
  }
  // print second row
  g_lcd.setCursor(0, 1);
  g_lcd.print(g_fan_level);
  PrintSpaceAndO(kLcdCols - 1, second_row_print_number);
  ControlFan();
}

// 控制风扇速度
void ControlFan() {
  int fan_speed = kFanLevelFactor * g_fan_level;
  if (fan_speed > kMaxFanLevel) {
    fan_speed = kMaxFanLevel;
  }
  analogWrite(kMotorIn2, fan_speed);
}
