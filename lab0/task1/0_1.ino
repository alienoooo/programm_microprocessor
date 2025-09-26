unsigned long last_time;

void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  bool is_light = digitalRead(13);
  if (millis() - last_time > 500) {
    last_time = millis();
    digitalWrite(13, !is_light);
  }
}
