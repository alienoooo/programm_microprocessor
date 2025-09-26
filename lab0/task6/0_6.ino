void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  bool power_button_status = !digitalRead(8);
  bool changer_button_status = !digitalRead(2);
  unsigned int press_count;
  unsigned int resistanse;
  if (!changer_button_status) {
    resistanse = analogRead(A0) / 4;
  }
  else {
    Serial.print("Brightness change not avaible");
  }
  Serial.println(resistanse);
  if (power_button_status) {
    press_count = 1 - press_count;
  }
  analogWrite(3, press_count * resistanse);
  if (press_count) {
    Serial.println("Button pressed");
  }
  else {
    Serial.println("Button not pressed");
  }
}
