// === 4×7-seg через 74HC595/мультиплату (ACTIVE LOW) ===
// Сегменты: 0 = включен, 1 = выключен
// Разряды:  0 = выбран,  1 = не выбран

#define LATCH_DIO 4   // ST_CP (latch)
#define CLK_DIO   7   // SH_CP (clock)
#define DATA_DIO  8   // DS    (data)

// Коды цифр 0..9 для 7-seg (active LOW). Порядок битов платы — "стандартный" для многих шилдов.
const uint8_t SEG_DIGITS[10] = {
  0xC0, // 0
  0xF9, // 1
  0xA4, // 2
  0xB0, // 3
  0x99, // 4
  0x92, // 5
  0x82, // 6
  0xF8, // 7
  0x80, // 8
  0x90  // 9
};

// Маски выбора разряда (active LOW): 0xF1 → активирован D1, и т.д.
// Если порядок цифр на табло другой — поменяй порядок элементов в этом массиве.
const uint8_t DIG_SEL[4] = {
  0xF1, // разряд 0 (левый, тысячи)
  0xF2, // разряд 1 (сотни)
  0xF4, // разряд 2 (десятки)
  0xF8  // разряд 3 (правый, единицы)
};

unsigned long counter_next;
const unsigned int counter_to = 200; // шаг счётчика каждые 200 мс
uint16_t counter = 0;

static inline void hc595Write(uint8_t segByte, uint8_t digitMask) {
  // Часто нужно сначала послать байт сегментов, потом байт разряда. Если табло «молчит» — поменяй местами эти две строки.
  digitalWrite(LATCH_DIO, LOW);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, segByte);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, digitMask);
  digitalWrite(LATCH_DIO, HIGH);
}

// Отобразить число на 4 разряда (ведущие нули остаются)
static inline void display4(uint16_t val) {
  uint8_t d[4] = {
    (uint8_t)((val/1000)%10),
    (uint8_t)((val/100)%10),
    (uint8_t)((val/10)%10),
    (uint8_t)(val%10)
  };

  // Быстрый мультиплекс: по кадру на каждый разряд.
  // Для равномерной яркости полезно сделать краткое "гашение" между кадрами — здесь это реализовано тем,
  // что мы каждый кадр явно формируем новый селектор разряда (остальные не выбраны).
  for (uint8_t i = 0; i < 4; ++i) {
    hc595Write(SEG_DIGITS[d[i]], DIG_SEL[i]);  // показать цифру на разряде i
    delayMicroseconds(900);                    // удержание ~0.9 мс (≈ 277 Гц на весь дисплей)
    // «Гашение» (снять выбор разряда) — полезно для борьбы с «призраками»:
    hc595Write(0xFF, 0xFF);                    // все сегменты выкл, ни один разряд не выбран
    delayMicroseconds(40);
  }
}

void setup() {
  pinMode(LATCH_DIO, OUTPUT);
  pinMode(CLK_DIO,   OUTPUT);
  pinMode(DATA_DIO,  OUTPUT);

  digitalWrite(LATCH_DIO, HIGH);
  digitalWrite(CLK_DIO,   LOW);
  digitalWrite(DATA_DIO,  LOW);

  counter_next = millis();     // ИНИЦИАЛИЗИРУЕМ, чтобы счётчик начал тикать корректно
}

void loop() {
  // Обновляем значение счётчика раз в 200 мс
  unsigned long tm = millis();
  if (tm >= counter_next) {
    counter_next = tm + counter_to;
    counter = (counter + 1) % 10000; // 0000..9999
  }

  // ВАЖНО: дисплей надо постоянно "освежать" мультиплексом, поэтому display4() вызываем КАЖДЫЙ loop
  display4(counter);
}