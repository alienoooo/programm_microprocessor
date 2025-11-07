const int ledPin = 13;          
const int buttonPin = 2;        

volatile bool fastMode = false; // Флаг для переключения режима мигания
volatile unsigned long lastDebounceTime = 0; 
const unsigned long debounceDelay = 50;      

void handleButtonPress() {
  if (millis() - lastDebounceTime > debounceDelay) {
    fastMode = !fastMode; // Переключаем режим мигания
    lastDebounceTime = millis();
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Используем внутренний подтягивающий резистор
  
  // Подключаем прерывание на D2 (FALLING - при нажатии кнопки)
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);
}

void loop() {
  unsigned long interval = fastMode ? 100 : 500; 
  
  digitalWrite(ledPin, HIGH);
  delay(interval);
  digitalWrite(ledPin, LOW);
  delay(interval);
}