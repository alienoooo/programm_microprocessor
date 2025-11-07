const int ledPin = 13;  // Светодиод подключен к D13

void setup() {
  pinMode(ledPin, OUTPUT);
  
  // 1. Сбрасываем регистры управления таймером
  TCCR1A = 0;  // Сбрасываем регистр TCCR1A
  TCCR1B = 0;  // Сбрасываем регистр TCCR1B
  
  // 2. Устанавливаем режим CTC (Clear Timer on Compare Match)
  // WGM12 = 1 (бит 3 в TCCR1B) для режима CTC
  TCCR1B |= (1 << WGM12);
  
  // 3. Устанавливаем значение для сравнения (TOP value)
  // Расчет: OCR1A = (F_CPU) / (N * F_int) - 1
  // F_CPU = 16,000,000 Гц, N = 64 (предделитель), F_int = 100 Гц
  // OCR1A = 16,000,000 / (64 * 100) - 1 = 2500 - 1 = 2499
  OCR1A = 2499;
  
  // 4. Включаем прерывание по совпадению с регистром A (Output Compare A Match Interrupt)
  TIMSK1 |= (1 << OCIE1A);
  
  // 5. Запускаем таймер с предделителем 64
  // CS10 = 1, CS11 = 1, CS12 = 0 
  TCCR1B |= (1 << CS11) | (1 << CS10);
  
  // Включаем глобальные прерывания
  sei();
}

// Обработчик прерывания по совпадению с регистром A Timer1
ISR(TIMER1_COMPA_vect) {
  // Инвертируем состояние светодиода каждые 10 мс
  static bool ledState = false;
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
}

void loop() {
  // Основная программа может выполнять другие задачи
  // или просто "спать", так как мигание светодиода
}