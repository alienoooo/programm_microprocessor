// ===== Lab2_FSM_SwitchCase.ino =====
/*
  Автосветофор: классический FSM (switch-case), неблокирующий.
  Пины: R=13, Y=12, G=11, BTN=2 (INPUT_PULLUP)
*/

enum State { S_GREEN, S_YELLOW, S_RED, S_WARNING };
State currentState = S_GREEN;

const byte PIN_R = 13, PIN_Y = 12, PIN_G = 11, PIN_BTN = 2;
unsigned long stateStart = 0, stateDur = 0;
bool pedRequest = false;     // пешеходный запрос
bool emergency = false;      // аварийный режим
bool nightMode = false;      // ночной режим (опция)

// тайминги (мс)
const unsigned long T_GREEN = 10000UL;
const unsigned long T_YELLOW = 3000UL;
const unsigned long T_RED = 10000UL;
const unsigned long T_PED_RED_EXTRA = 5000UL; // доп. красный для пешехода
const unsigned long T_WARNING_BLINK = 500UL;  // период мигания
const unsigned long T_NIGHT_BLINK   = 1000UL; // ночной режим

// debounce
unsigned long lastBtnChange = 0;
const unsigned long DEBOUNCE_MS = 50;
int lastBtnState = HIGH; // INPUT_PULLUP

void setOutputsForState(State s) {
  // сброс
  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_Y, LOW);
  digitalWrite(PIN_G, LOW);

  switch (s) {
    case S_GREEN:   digitalWrite(PIN_G, HIGH); break;
    case S_YELLOW:  digitalWrite(PIN_Y, HIGH); break;
    case S_RED:     digitalWrite(PIN_R, HIGH); break;
    case S_WARNING: /* мигаем в loop() */      break;
  }
}

void goToState(State s, unsigned long dur) {
  currentState = s;
  stateStart = millis();
  stateDur = dur;
  setOutputsForState(s);
  Serial.print("-> "); Serial.print((int)s);
  Serial.print(" for "); Serial.println(dur);
}

void beginNormalCycle()     { goToState(S_GREEN, T_GREEN); }
void beginWarningMode()     { goToState(S_WARNING, T_WARNING_BLINK); }
void beginNightModeBlink()  { goToState(S_WARNING, T_NIGHT_BLINK); }

void readInputs() {
  // Эмуляторы/входы: кнопка = PED_REQ / toggle night / toggle emergency
  int raw = digitalRead(PIN_BTN);
  unsigned long now = millis();
  if (raw != lastBtnState && now - lastBtnChange > DEBOUNCE_MS) {
    lastBtnChange = now;
    lastBtnState = raw;
    if (raw == LOW) {
      // короткое нажатие = пешеходный запрос
      pedRequest = true;
      Serial.println("E_PED_REQ");
    }
  }
  // Для демонстрации: простые сериал-команды
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'e') { emergency = true;  Serial.println("E_EMERGENCY_ON"); }
    if (c == 'q') { emergency = false; Serial.println("E_EMERGENCY_OFF"); }
    if (c == 'n') { nightMode = !nightMode; Serial.println("E_NIGHT_TOGGLE"); }
  }
}

void setup() {
  pinMode(PIN_R, OUTPUT); pinMode(PIN_Y, OUTPUT); pinMode(PIN_G, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  Serial.begin(115200);
  beginNormalCycle();
}

void loop() {
  readInputs(); // обновляет pedRequest/emergency/nightMode  (неблокирующе)

  // аварийный приоритет: немедленно в WARNING, пока активен
  if (emergency && currentState != S_WARNING) {
    beginWarningMode(); // мигающий жёлтый
  }

  unsigned long now = millis();

  switch (currentState) {
    case S_GREEN:
      if (now - stateStart >= stateDur) {
        // завершение зелёного -> жёлтый
        goToState(S_YELLOW, T_YELLOW);
      }
      break;

    case S_YELLOW:
      if (now - stateStart >= stateDur) {
        // если был пешеходный запрос — даём удлинённый красный
        unsigned long redTime = pedRequest ? (T_RED + T_PED_RED_EXTRA) : T_RED;
        pedRequest = false;
        goToState(S_RED, redTime);
      }
      break;

    case S_RED:
      if (now - stateStart >= stateDur) {
        // если активирован ночной режим — перейдём в мигающий жёлтый (1 c)
        if (nightMode) beginNightModeBlink();
        else           goToState(S_GREEN, T_GREEN);
      }
      break;

    case S_WARNING: {
      // Мигание жёлтого: раз в stateDur меняем Y-светодиод
      static bool yOn = false;
      if (now - stateStart >= stateDur) {
        stateStart = now;
        yOn = !yOn;
        digitalWrite(PIN_R, LOW);
        digitalWrite(PIN_G, LOW);
        digitalWrite(PIN_Y, yOn ? HIGH : LOW);
      }
      // Выход из WARNING: если авария снята и ночной режим не активен — возвращаемся в цикл
      if (!emergency && !nightMode) {
        goToState(S_GREEN, T_GREEN);
      }
      // Если авария снята, но включён ночной — остаёмся в мигающем с периодом T_NIGHT_BLINK
      if (!emergency && nightMode && stateDur != T_NIGHT_BLINK) {
        goToState(S_WARNING, T_NIGHT_BLINK);
      }
      break;
    }
  }
}