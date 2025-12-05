#include <LiquidCrystal_I2C.h>

const int LDR_PIN = A0;        // фоторезистор
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LDR calib");
}

void loop() {
  int adc = analogRead(LDR_PIN);

  // На дисплей
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADC=");
  lcd.print(adc);

  lcd.setCursor(0, 1);
  lcd.print("Lux: read ref"); 

  // В Serial для протокола
  Serial.print("ADC=");
  Serial.println(adc);

  delay(1000);   // раз в секунду
}