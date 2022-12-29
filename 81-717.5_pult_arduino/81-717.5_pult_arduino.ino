constexpr short latchPin = 11; //Пин подключен к ST_CP входу 74HC595
constexpr short clockPin = 12; //Пин подключен к SH_CP входу 74HC595
constexpr short dataPin = 10; //Пин подключен к DS входу 74HC595

constexpr short totalRegisters = 8; // Общее число подключенных регистров

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT); 
  pinMode(clockPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available()) {
    //registersWrite("1111111100000000000000010000000000000001000000010000000000000000");
    registersWrite(Serial.readString());
  }
}

void registersWrite(String toSend)
{   
  digitalWrite(latchPin, LOW); 

  for(short currentRegisterNumber = 0; currentRegisterNumber < totalRegisters; currentRegisterNumber++) {
    byte currentRegisterByte = B00000000;
      for(short k = 0; k < 8; k++) bitWrite(currentRegisterByte, k, toSend[currentRegisterNumber*8 + k]-'0');
      shiftOut(dataPin, clockPin, MSBFIRST, currentRegisterByte);     
  }

  digitalWrite(latchPin, HIGH);
}