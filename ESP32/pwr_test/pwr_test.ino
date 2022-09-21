#define POWERPIN  12

byte state = 1;

void setup() {
  pinMode(POWERPIN, OUTPUT);
}

void loop() {
  digitalWrite(POWERPIN, state);
  delay(5000);
  state = !state;
}
