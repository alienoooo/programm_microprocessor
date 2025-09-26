float V = 1.0;
float S = 1.0;
float signal;
float angle;

float getSignal(float angle) {
    if (angle < 120.0) {
      signal = V * (1.0 - S) * 255.0;
    }
    else if (angle < 180.0) {
      signal = (V * (1.0 - S) + (angle - 120.0) / 60.0 * V) * 255.0;
    }
    else if (angle < 300.0) {
      signal = V * 255.0;
    }
    else {
      signal = (V - (angle - 300.0) / 60.0 * V) * 255.0;
    }
    return signal;
  }

void setup() {
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A0, INPUT);
}

void loop() {
  angle = analogRead(A0) / 1000.0 * 360.0;
  analogWrite(11, getSignal(int(angle + 240.0) % 360));
  analogWrite(10, getSignal(int(angle + 120.0) % 360));
  analogWrite(9, getSignal(angle));
}
