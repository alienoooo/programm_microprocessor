void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(8, INPUT_PULLUP);
}

void loop() {
  bool button_status = !digitalRead(8);
  bool press_count;
  if (button_status) {
    press_count = !press_count;
  }
  digitalWrite(13, press_count);
  if (press_count) {
    Serial.println("Button pressed");
  }
  else {
    Serial.println("Button not pressed");
  }
}
