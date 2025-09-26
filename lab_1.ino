
const uint8_t INPUT_MASK = (1 << PD4) | (1 << PD2) | (1 << PD3);

const uint8_t OUTPUT_MASK =  (1 << PB5) | (1 << PB4) | (1 << PB3);

// Печать байта в бинарном виде
void printByteBinary(uint8_t b) {
  Serial.print("0b");
  for (int i = 7; i >= 0; i--) {
    Serial.print((b >> i) & 1);
    if (i > 0) Serial.print("");
  }
}

// Простая функция debounce: 4 стабильных чтения с 10ms интервалом
uint8_t readButtonsDebounced() {
  const int samples = 4;
  const int delayMs = 10;
  uint8_t last = (~PIND) & INPUT_MASK;
  for (int i = 0; i < samples; i++) {
    delay(delayMs);
    uint8_t cur = (~PIND) & INPUT_MASK;
    if (cur != last) {
      i = 0;       // сброс счетчика, если изменилось
      last = cur;
    }
  }
  return last;
}

// Функция поиска позиции первого установленного бита (через цикл, младший бит)
int firstSetBitLoop(uint8_t b) {
  for (int i = 0; i < 8; i++) {
    if (b & (1 << i)) return i;
  }
  return -1; // нет установленных битов
}

// Функция поиска позиции первого установленного бита через __builtin_ctz
int firstSetBitBuiltin(uint8_t b) {
  if (b == 0) return -1;
  return __builtin_ctz(b); 
}

void setup() {
  Serial.begin(9600);

  // Настройка портов
  DDRD &= ~INPUT_MASK;  
  PORTD |= INPUT_MASK;   
  DDRB |= OUTPUT_MASK;  
  PORTB &= ~OUTPUT_MASK; 
  
  Serial.println("Программа работы с кнопками и светодиодами");
}

void loop() {
  // 2.6. Чтение кнопок с debounce
  uint8_t pressed = readButtonsDebounced();
  pressed &= INPUT_MASK;
  pressed = ~pressed & INPUT_MASK;
  // 2.2. Вывод всего байта порта D в бинарном виде
  Serial.print("PIND: ");
  printByteBinary(PIND);

  // 2.3. Определяем, какие кнопки нажаты и выводим позиции
  Serial.print(" Нажаты кнопки (позиции битов PD): ");
  for (uint8_t i = 0; i < 8; i++) {
    if (pressed & (1 << i)) {
      Serial.print(i);
      Serial.print("");
    }
  }

  // 2.4. Сформировать байт для светодиодов
  // способ 1: через маску
  uint8_t led_byte = 0;
  if (pressed & (1 << PD2)) led_byte |= (1 << PB5);
  if (pressed & (1 << PD3)) led_byte |= (1 << PB4);
  if (pressed & (1 << PD4)) led_byte |= (1 << PB3);

  // способ 2: через bitSet (демонстрация)
  if (pressed & (1 << PD2)) bitSet(led_byte, PB5);
  if (pressed & (1 << PD3)) bitSet(led_byte, PB4);
  if (pressed & (1 << PD4)) bitSet(led_byte, PB3);

  // Запись на PORTB, не меняя остальные биты
  PORTB = (PORTB & ~OUTPUT_MASK) | (led_byte & OUTPUT_MASK);

  // Вывод байта LED в терминал
  Serial.print(" | LED byte: ");
  printByteBinary(led_byte);

  // 2.5. Позиция первого установленного бита
  int first_loop = firstSetBitLoop(pressed);
  int first_builtin = firstSetBitBuiltin(pressed);
  Serial.print(" | First set bit (loop): ");
  Serial.print(first_loop);
  Serial.print(" | First set bit (__builtin_ctz): ");
  Serial.println(first_builtin);

  delay(200); 
}