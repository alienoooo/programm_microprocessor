void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(8, INPUT_PULLUP);
}

void loop() {
  bool button_status = digitalRead(8);
  digitalWrite(13, button_status);
  if (button_status) {
    Serial.println("Button not pressed");
  }
  else {
    Serial.println("Button pressed");
  }
}
