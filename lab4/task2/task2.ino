#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// --- Пины резистивного тача для mcufriend shield (UNO):
// X+ = D6, X- = A2, Y+ = A1, Y- = D7
const int XP = 6, XM = A2, YP = A1, YM = 7;
const int TS_RESISTOR = 300;         // номинал пленки (примерно)

// Порог нажатия (давление)
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// КАЛИБРОВКА (под 320x240; начальные значения типичные — при необходимости подправь)
int TS_LEFT = 900, TS_RIGHT = 120, TS_TOP = 90, TS_BOT = 920;

TouchScreen ts(XP, YP, XM, YM, TS_RESISTOR);

void setup() {
  Serial.begin(9600);

  uint16_t id = tft.readID();   // автоопределение контроллера
  if (id == 0xD3 || id == 0xFFFF) id = 0x9341; // fallback для «капризных» клонов
  tft.begin(id);

  tft.setRotation(1);           // 1 = ландшафт (ширина 320, высота 240)
  tft.fillScreen(0x0000);       // черный фон

  // Простейшая графика
  tft.fillRect(0,   0, 320, 30,  0x07E0);  // зелёная шапка
  tft.setTextColor(0xFFFF); tft.setTextSize(2);
  tft.setCursor(8,6); tft.print("TFT 320x240: Demo");

  tft.drawRect(0, 30, 320, 210, 0xFFFF);   // рабочая область
  tft.setCursor(8, 216); tft.setTextSize(1);
  tft.print("Touch to draw. Press harder -> bigger dot.");

  // цветные полосы для проверки
  for (int x=0; x<320; x+=40) tft.fillRect(x, 40, 40, 160, (x/40)%2 ? 0xF800 : 0x001F);
}

void loop() {
  // Читаем тач
  TSPoint p = ts.getPoint();
  // Вернём линии на выводы, которые используются дисплеем:
  pinMode(XM, OUTPUT); pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // map() из «сырых» координат тача в пиксели экрана (с учётом калибровки)
    int16_t x = map(p.y, TS_LEFT, TS_RIGHT, 0, 320);   // при rotation=1 так удобнее
    int16_t y = map(p.x, TS_TOP,  TS_BOT,   0, 240);

    // ограничим рамками
    if (x < 0) x = 0; if (x >= 320) x = 319;
    if (y < 0) y = 0; if (y >= 240) y = 239;

    // Радиус точки зависит от силы нажатия
    int r = 2 + (p.z - MINPRESSURE) / 150;
    if (r > 8) r = 8;

    // Рисуем внутри рабочей зоны
    if (y > 32 && y < 220) {
      tft.fillCircle(x, y, r, 0xF81F); // пурпурная точка
    }
  }
}