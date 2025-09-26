#define n 10

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  digitalWrite(11, 1);
  delay((n - 3) * 1000);

  digitalWrite(11, 0);
  delay(500);
  digitalWrite(11, 1);
  delay(500);
  digitalWrite(11, 0);
  delay(500);
  digitalWrite(11, 1);
  delay(500);
  digitalWrite(11, 0);
  delay(500);
  digitalWrite(11, 1);
  delay(500);
  digitalWrite(11, 0);

  digitalWrite(12, 1);
  delay(1000);
  digitalWrite(12, 0);

  digitalWrite(13, 1);
  delay((n - 1) * 1000);
  digitalWrite(12, 1);
  delay(1000);
  digitalWrite(13, 0);
  digitalWrite(12, 0);
}
