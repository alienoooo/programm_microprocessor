// ===== Lab2_FSM_TableDriven.ino =====
/*
  Автосветофор: табличный FSM (table-driven).
  Пины: R=13, Y=12, G=11, BTN=2 (INPUT_PULLUP)
*/

enum State { S_GREEN, S_YELLOW, S_RED, S_WARNING, NUM_STATES };
enum Event { E_NONE, E_TIMER, E_PED, E_EMERGENCY_ON, E_EMERGENCY_OFF, E_NIGHT_TOGGLE, NUM_EVENTS };

const byte PIN_R = 13, PIN_Y = 12, PIN_G = 11, PIN_BTN = 2;
unsigned long stateStart = 0, stateDur = 0;
State currentState = S_GREEN;

const unsigned long T_GREEN = 10000UL, T_YELLOW = 3000UL, T_RED = 10000UL;
const unsigned long T_PED_RED_EXTRA = 5000UL;
const unsigned long T_WARNING_BLINK = 500UL, T_NIGHT_BLINK = 1000UL;

bool pedPending = false, emergency = false, nightMode = false;

// debounce
unsigned long lastBtnChange = 0; const unsigned long DEBOUNCE_MS = 50;
int lastBtnState = HIGH;

typedef void (*Handler)(void);
Handler fsmTable[NUM_STATES][NUM_EVENTS];

// функция для красивого вывода состояния
void printStateName(State s) {
  switch (s) {
    case S_GREEN:   Serial.println("STATE: GREEN (зеленый свет)"); break;
    case S_YELLOW:  Serial.println("STATE: YELLOW (желтый свет)"); break;
    case S_RED:     Serial.println("STATE: RED (красный свет)"); break;
    case S_WARNING: Serial.println("STATE: WARNING (мигающий желтый)"); break;
    default:        Serial.println("STATE: UNKNOWN"); break;
  }
}

void setOutputs(State s) {
  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_Y, LOW);
  digitalWrite(PIN_G, LOW);
  if (s == S_GREEN) digitalWrite(PIN_G, HIGH);
  if (s == S_YELLOW) digitalWrite(PIN_Y, HIGH);
  if (s == S_RED) digitalWrite(PIN_R, HIGH);
}

void toState(State s, unsigned long dur) {
  currentState = s;
  stateDur = dur;
  stateStart = millis();
  setOutputs(s);
  Serial.print("-> "); Serial.print((int)s);
  Serial.print(" for "); Serial.println(dur);
  printStateName(s); // 🔹 добавлен вывод имени состояния
}

// ===== Handlers =====
void onTimer_GREEN()  { toState(S_YELLOW, T_YELLOW); }                                // таймер зелёного
void onTimer_YELLOW() {
  unsigned long r = pedPending ? T_RED + T_PED_RED_EXTRA : T_RED;
  pedPending = false;
  toState(S_RED, r);
}
void onTimer_RED()    { if (nightMode) toState(S_WARNING, T_NIGHT_BLINK);
                        else toState(S_GREEN, T_GREEN); }

void onAny_EMERGENCY_ON()  { toState(S_WARNING, T_WARNING_BLINK); }                   // приоритет аварии
void onAny_EMERGENCY_OFF() { if (!nightMode) toState(S_GREEN, T_GREEN);
                             else toState(S_WARNING, T_NIGHT_BLINK); }
void onAny_NIGHT_TOGGLE()  { nightMode = !nightMode;
                             if (nightMode) toState(S_WARNING, T_NIGHT_BLINK);
                             else if (!emergency) toState(S_GREEN, T_GREEN); }

void onPedIn_GREEN() { pedPending = true; Serial.println("ped queued"); }             // регистрируем запрос

void setupTable() {
  for (int s=0; s<NUM_STATES; ++s)
    for (int e=0; e<NUM_EVENTS; ++e)
      fsmTable[s][e] = NULL;

  // таймерные переходы
  fsmTable[S_GREEN][E_TIMER]  = onTimer_GREEN;
  fsmTable[S_YELLOW][E_TIMER] = onTimer_YELLOW;
  fsmTable[S_RED][E_TIMER]    = onTimer_RED;

  // пешеходный запрос учитываем в Green
  fsmTable[S_GREEN][E_PED] = onPedIn_GREEN;

  // авария и ночной режим для всех состояний
  for (int s=0; s<NUM_STATES; ++s) {
    fsmTable[s][E_EMERGENCY_ON]  = onAny_EMERGENCY_ON;
    fsmTable[s][E_EMERGENCY_OFF] = onAny_EMERGENCY_OFF;
    fsmTable[s][E_NIGHT_TOGGLE]  = onAny_NIGHT_TOGGLE;
  }
}

Event pollEvent() {
  // debounce кнопки → E_PED
  int raw = digitalRead(PIN_BTN);
  unsigned long now = millis();
  if (raw != lastBtnState && now - lastBtnChange > DEBOUNCE_MS) {
    lastBtnChange = now; lastBtnState = raw;
    if (raw == LOW) return E_PED;
  }

  // сериал-команды
  if (Serial.available()) {
    char c = Serial.read();
    if (c=='e') { emergency=true;  return E_EMERGENCY_ON;  }
    if (c=='q') { emergency=false; return E_EMERGENCY_OFF; }
    if (c=='n') { return E_NIGHT_TOGGLE; }
  }

  // таймер истёк?
  if (now - stateStart >= stateDur) return E_TIMER;
  return E_NONE;
}

void setup() {
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_Y, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  Serial.begin(115200);
  setupTable();                                         // таблица состояний/событий
  toState(S_GREEN, T_GREEN);
}

void loop() {
  Event ev = pollEvent();                               // 1 событие за итерацию
  if (ev != E_NONE) {
    Handler h = fsmTable[currentState][ev];
    if (h) h();                                         // обработка может менять состояние/таймер
  }

  // WARNING мигает: тут — немодальный выход; индикатор Y переключаем сам по себе
  if (currentState == S_WARNING) {
    static bool yOn = false; unsigned long now = millis();
    if (now - stateStart >= stateDur) {
      stateStart = now;
      yOn = !yOn;
      digitalWrite(PIN_Y, yOn ? HIGH : LOW);
    }
  }
}