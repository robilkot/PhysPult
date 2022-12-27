void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  for(int i = 2; i <= 13; i++) pinMode (i, OUTPUT);
}

void loop() {
  String state = Serial.readString(); // 0 - dont change, 1 - turn on, 2 - turn off
  
  for(int i = 6; i <= 13; i++) {
    switch(state[i]) {
    case 1: digitalWrite(i-4, 1); break;
    case 2: digitalWrite(i-4, 0); break;
    }
  }
}