const short PINNUMBER = 13;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  for(int i = 2; i <= PINNUMBER; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, 1);
  }
  delay(1000);
  for(int i = 2; i <= PINNUMBER; i++) digitalWrite(i, 0);
}

void loop() {
  String state = Serial.readString(); // 0 - don't change, 1 - turn on, 2 - turn off
  
  for(int i = 6; i <= PINNUMBER; i++) {
    switch(state[i]) {
    case '1': digitalWrite(i-4, 1); break;
    case '2': digitalWrite(i-4, 0); break;
    }
  }
}