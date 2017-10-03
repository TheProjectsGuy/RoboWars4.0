#define ENERGISE_RELAY_BY_LOGIC_LEVEL HIGH

void setup() {
  pinMode(7, OUTPUT);
}

void loop() {
  //digitalWrite(7, ENERGISE_RELAY_BY_LOGIC_LEVEL);
  digitalWrite(7, !ENERGISE_RELAY_BY_LOGIC_LEVEL);
}

