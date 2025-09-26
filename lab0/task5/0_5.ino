void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  pinMode(A0, INPUT);
}

void loop() {
  bool button_status = !digitalRead(8);
  unsigned int press_count;
  unsigned int resistanse = analogRead(A0) / 4;
  Serial.println(resistanse);
  if (button_status) {
    press_count = 1 - press_count;
  }
  analogWrite(6, press_count * resistanse);
  if (press_count) {
    Serial.println("Button pressed");
  }
  else {
    Serial.println("Button not pressed");
  }
}
