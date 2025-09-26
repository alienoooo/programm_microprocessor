#define n 10

void setup() {
  pinMode(6, INPUT_PULLUP);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  bool button_status = !digitalRead(6);
  digitalWrite(11, 1);
  if(button_status) {
    for (unsigned int time = 0; time < 2; time++) {
      Serial.print(5 - time);
      delay(1000);
    }
    bool is_light = 0;
    for (unsigned int i = 0; i < 6; i++) {
      digitalWrite(11, is_light);
      if (!is_light) {
        Serial.print(n - 7 - i / 2);
      }
      is_light = !is_light;
      delay(500);
    }

    digitalWrite(13, 1);
    delay(1000);
    digitalWrite(11, 0);

    for (int time = 0; time < n - 1; time++) {
      Serial.print(n - time);
      delay(1000);
    }
    digitalWrite(11, 1);
    Serial.print("1");
    delay(1000);
    digitalWrite(13, 0);
  }
}
