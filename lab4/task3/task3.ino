// === ЛР4: Задание 3 — 4×7-seg, общий катод (CC), транзисторы NPN (low-side) ===
// Разряд включается уровнем HIGH
#define DIGIT_ENABLE_IS_HIGH 1

// Тайминги (можно подстроить при необходимости)
#define ON_US       1200   // выдержка включенного разряда (600–1200 мкс)
#define BLANK_US     50   // короткое гашение между разрядами
#define SETTLE_US    10   // пауза на установку сегментов

// Пины сегментов в ПОРЯДКЕ: A, B, C, D, E, F, G, DP  (твоя распиновка)
const uint8_t segPins[8]   = {12, 10, 6, 8, 9, 11, 5, 7};
// Пины разрядов слева→направо (тысячи, сотни, десятки, единицы)
// ВАЖНО: не используем D1 (TX), вместо него D13
const uint8_t digitPins[4] = {13, 2, 3, 4};

// Таблица сегментов (CC): биты в порядке a..g (1 = сегмент ВКЛ)
const uint8_t DIGITS_A2G[10] = {
  //  a  b  c  d  e  f  g
  0b0111111, // 0
  0b0000110, // 1
  0b1011011, // 2
  0b1001111, // 3
  0b1100110, // 4
  0b1101101, // 5
  0b1111101, // 6
  0b0000111, // 7
  0b1111111, // 8
  0b1101111  // 9
};

inline void allDigitsOff() {
  // Для NPN-low-side: LOW = разряд выключен, HIGH = включен
  for (uint8_t i=0;i<4;i++) digitalWrite(digitPins[i], LOW);
}
inline void setDigit(uint8_t idx, bool on){
  digitalWrite(digitPins[idx], on ? HIGH : LOW);
}
inline void setSegments(uint8_t patA2G){
  // CC: HIGH включает сегмент
  for (int s=0; s<7; ++s) digitalWrite(segPins[s], (patA2G >> s) & 1 ? HIGH : LOW);
  digitalWrite(segPins[7], LOW); // DP выключена
}

void setup(){
  // Не используем Serial.begin(), чтобы UART не дёргал D1/TX
  for (auto p: segPins)   pinMode(p, OUTPUT);
  for (auto p: digitPins) pinMode(p, OUTPUT);
  allDigitsOff();
  pinMode(A0, INPUT);
}

inline void show4(uint16_t v){
  // тысяч–сотен–десятков–единиц (слева→направо)
  uint8_t d[4] = { (uint8_t)((v/1000)%10),
                   (uint8_t)((v/100)%10),
                   (uint8_t)((v/10)%10),
                   (uint8_t)(v%10) };

  for (uint8_t i=0;i<4;i++){
    // 1) гарантированно гасим все разряды
    allDigitsOff();
    delayMicroseconds(BLANK_US);

    // 2) ставим сегменты будущей цифры, даём им «устаканиться»
    setSegments(DIGITS_A2G[d[i]]);
    delayMicroseconds(SETTLE_US);

    // 3) включаем ТОЛЬКО текущий разряд
    setDigit(i, true);
    delayMicroseconds(ON_US);

    // 4) выключаем (перейдём к следующему)
    setDigit(i, false);
  }
}

void loop(){
  uint16_t val = analogRead(A0);   // 0000…1023
  show4(val);
}