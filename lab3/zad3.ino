// Выводы для клавиатуры 4x4
const int rowPins[4] = {7, 6, 5, 4};
const int colPins[4] = {11, 10, 9, 8};

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

volatile byte currentRow = 0;
volatile char lastKey = 0;
volatile bool keyPressed = false;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(9600);
  
  // Настройка выводов клавиатуры
  for (int i = 0; i < 4; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
    pinMode(colPins[i], INPUT_PULLUP);
  }
  
  // Настройка Timer2 для прерывания каждые 1 мс
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A |= (1 << WGM21);  // Режим CTC
  OCR2A = 249;             // 16MHz/64/1000Hz - 1 = 249
  TIMSK2 |= (1 << OCIE2A); // Разрешить прерывание
  TCCR2B |= (1 << CS22);   // Предделитель 64
  
  sei();
}

ISR(TIMER2_COMPA_vect) {
  // Деактивируем предыдущую строку
  digitalWrite(rowPins[currentRow], HIGH);
  
  // Переходим к следующей строке
  currentRow = (currentRow + 1) % 4;
  
  // Активируем новую строку
  digitalWrite(rowPins[currentRow], LOW);
  
  // Читаем столбцы
  for (int col = 0; col < 4; col++) {
    if (digitalRead(colPins[col]) == LOW) {
      // Защита от дребезга
      if (millis() - lastDebounceTime > 50) {
        char newKey = keys[currentRow][col];
        if (newKey != lastKey) {
          lastKey = newKey;
          keyPressed = true;
          lastDebounceTime = millis();
        }
      }
      break;
    }
  }
}

void loop() {
  if (keyPressed) {
    Serial.print("Клавиша обработана: ");
    Serial.println(lastKey);
    keyPressed = false;
  }
}