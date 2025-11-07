#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();          // инициализация LCD
  lcd.backlight();     // включить подсветку
  lcd.clear();

  // верхняя строка — фамилия
  lcd.setCursor(0, 0);
  lcd.print("Radchenko");

  // нижняя строка — подпись Temp и стартовое значение
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
}

void loop() {
  // имитация изменения температуры
  static float temp = 36.5;
  temp += 0.1;
  if (temp > 39.5) temp = 36.5;

  // обновляем значение после "Temp: "
  lcd.setCursor(6, 1);      // после пробела
  lcd.print(temp, 1);       // 1 знак после запятой
  lcd.print((char)223);     // символ ° (градус)
  lcd.print('C');
  lcd.print(' ');           // затираем возможные хвосты

  delay(1000);
}