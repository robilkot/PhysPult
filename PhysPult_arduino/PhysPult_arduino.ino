#include <FastLED.h>
#include <Servo.h>

uint8_t leftdigit[10][2] = {
{B00010000, B11101001},
{B00010000, B00001000},
{B00010000, B10110001},
{B00010000, B10111000},
{B00010000, B01011000},
{B00000000, B11111000},
{B00000000, B11111001},
{B00010000, B10001000},
{B00010000, B11111001},
{B00010000, B11111000}
};

uint8_t rightdigit[10][2] = {
{B11100010, B00000110},
{B11000000, B00000000},
{B10100001, B00000110},
{B11100001, B00000010},
{B11000011, B00000000},
{B01100011, B00000010},
{B01100011, B00000110},
{B11100000, B00000000},
{B11100011, B00000110},
{B11100011, B00000010}
};

const int pulseWidth = 10; // pulse width for work with registers (prevents from getting trash input)

const long BaudRate = 38400;

const uint8_t out_datapin = 2; // 71hc595 registers
const uint8_t out_clockpin = 4;
const uint8_t out_latchpin = 7;
const uint8_t out_registerscount = 10;

const uint8_t in_datapin = 8; // 71hc165 registers
const uint8_t in_clockpin = 12;
const uint8_t in_latchpin = 13;
const uint8_t in_registerscount = 6;

uint8_t indicators[out_registerscount];
uint8_t switches[in_registerscount];

const uint8_t voltmeter_pwmpin = 5;

const uint8_t tm_pwmpin = 9;
//const uint8_t nm_pwmpin = 9;
//const uint8_t tc_pwmpin = 9;

Servo tm_servo;

const uint8_t lighting_datapin = 3; // pult lighting
const uint8_t lighting_ledcount = 8;
uint8_t lighting_color[4] = { 30, 200, 255, 150 }; // HSV + brightness

CRGB leds[lighting_ledcount];

void setup()
{
  pinMode(out_datapin, OUTPUT);
  pinMode(out_clockpin, OUTPUT);
  pinMode(out_latchpin, OUTPUT);

  pinMode(in_datapin, INPUT);
  pinMode(in_clockpin, OUTPUT);
  pinMode(in_latchpin, OUTPUT);

  for(int i = 0; i < out_registerscount; i++)
    indicators[i] = 0;

  for(int i = 0; i < in_registerscount; i++)
    switches[i] = 0;

  pinMode(voltmeter_pwmpin, OUTPUT);
  voltmetershow(75);

  pinMode(tm_pwmpin, OUTPUT);
  tm_servo.attach(tm_pwmpin);

  pinMode(lighting_datapin, OUTPUT);

  FastLED.addLeds<WS2812B, lighting_datapin, GRB>(leds, lighting_ledcount).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
  // FastLED.setBrightness(lighting_color[3]);
  for (int i = 0; i < lighting_ledcount; i++ )
    leds[i] = CHSV(lighting_color[0], lighting_color[1], lighting_color[2]);
  
  FastLED.show();

  uint8_t on[5] = {88, 255, 255, 255, 255},
          off[5] = {0, 0, 0, 0, 0};

  updateIndicators(on);
  delay(500);
  updateIndicators(off);
  //FastLED.setBrightness(0);
  delay(100);

  Serial.begin(BaudRate);
  Serial.setTimeout(15);

  while(Serial.available()) Serial.read();
}

uint32_t timer = 0;
int pos = 0; 
bool direction = 0;

void loop()
{
  if(Serial.available()) 
  {
    if(Serial.read() != '{') return;
    
    // indicators[temp[0]-'0'] = atoi(temp.substring(1,temp.length()).c_str());
    // Serial.print(indicators[temp[0]-'0'], DEC);
    // Serial.print('\n');

    Serial.readBytesUntil('}', indicators, out_registerscount);

    //updateSwitches(switches);
    updateIndicators(indicators);

    Serial.write('{');

    Serial.write(switches, in_registerscount);
    //Serial.write(indicators, out_registerscount);
  
    Serial.write('}');
  }

  FastLED.show();

  // long davn = 
  // //Serial.parseInt(); 
  // //map(Serial.parseInt(), 0, 55, 0, 180);
  // map(Serial.parseInt(), 0, 153, 0, 180);
  // if(davn > 180 || davn < 0) return;

  //analogWrite(voltmeter_pwmpin, voltmeterpwm(150));

  if (millis() - timer >= 25)
  {
    updateSwitches(switches);

    if(switches[5] >> 4 & 1) FastLED.setBrightness(0);
    else {
      FastLED.setBrightness(lighting_color[3]);
    if(pos > 90) direction = 1;
    if(pos < 1) direction = 0;
    pos += direction ? -1 : 1;
    }
    
    tm_servo.write(pos); 

    do {
      timer += 25;
      if (timer < 25) break;  // переполнение uint32_t
    } while (timer < millis() - 25); // защита от пропуска шага
  }
}

void voltmetershow(uint8_t voltage) {
  voltage %= 151;
  analogWrite(voltmeter_pwmpin, map(voltage, 0, 151, 0, 26));
}

byte read165(uint8_t data, uint8_t clock)
{
  byte ret = 0;

  // The first one that is read is the highest bit (input D7 of the 74HC165).
  for(int i = 7; i >= 0; i--)
  {
    if(digitalRead(data) == HIGH)
      bitSet(ret, i);

    digitalWrite(clock, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(clock, LOW);
  }

  return ret;
}

void updateIndicators(byte* command)
{
  char speed = command[0];
  byte secondSpeedByte = leftdigit[speed / 10][0] | rightdigit[speed % 10][0] | command[1];
  byte firstSpeedByte = leftdigit[speed / 10][1] | rightdigit[speed % 10][1]; 

  digitalWrite(out_latchpin, 0);
  //*LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW
  delayMicroseconds(pulseWidth);

  shiftOut(out_datapin, out_clockpin, LSBFIRST, command[4]); // Регистр 5

  shiftOut(out_datapin, out_clockpin, LSBFIRST, command[2]); // Регистр 4
  shiftOut(out_datapin, out_clockpin, LSBFIRST, command[3]); // Регистр 3

  shiftOut(out_datapin, out_clockpin, LSBFIRST, secondSpeedByte); // Регистр 2
  shiftOut(out_datapin, out_clockpin, LSBFIRST, firstSpeedByte); // Регистр 1 

  delayMicroseconds(pulseWidth);
  //*LADDR |= 1 << latch - 8 * Lshift; // latch HIGH
  digitalWrite(out_latchpin, 1);
}

void updateSwitches(byte* output) {
  digitalWrite(in_latchpin, LOW);    
  delayMicroseconds(pulseWidth);
  digitalWrite(in_latchpin, HIGH);

  for(int i = 0; i < in_registerscount; i++)
    output[i] = read165(in_datapin, in_clockpin);
 
  //for(int k = 0; k < in_registerscount; k++) for(int i = 0; i < 8; i++) Serial.print(output[k] >> (7 - i) & 1);
  //Serial.println();            
}


// volatile uint8_t *DADDR = data > 7 ? &PORTB : &PORTD,
//                  *LADDR = latch > 7 ? &PORTB : &PORTD,
//                  *CADDR = clock > 7 ? &PORTB : &PORTD;
// bool Dshift = DADDR == &PORTB,
//      Lshift = LADDR == &PORTB,
//      Cshift = CADDR == &PORTB;
//   for (uint8_t i = 0; i < toSend.length(); i++)  {
//     for(int k = 0; k < 8; k++, toSend[i] >>= k) {
//       (toSend[i] & 1) ? *DADDR |= 1 << data - 8 * Dshift : *DADDR &= ~(1 << data - 8 * Dshift); // write 1 or 0 to corresponding bit
//       *CADDR |= 1 << clock - 8 * Cshift; // clock HIGH
//       *CADDR &= ~(1 << clock - 8 * Cshift); // clock LOW
//     }
//   }


//   volatile uint8_t *DADDR = data > 7 ? &PINB : &PIND,
//                    *LADDR = latch > 7 ? &PORTB : &PORTD,
//                    *CADDR = clock > 7 ? &PORTB : &PORTD;
//   bool Dshift = DADDR == &PINB,
//        Lshift = LADDR == &PORTB,
//        Cshift = CADDR == &PORTB;
//   *LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW
//   *LADDR |= 1 << latch - 8 * Lshift; // latch HIGH
//   for (uint8_t i = 0; i < totalRegisters * 8; i++) {
//     ret += *DADDR & (1 << data - 8 * Dshift) == 1 << data - 8 * Dshift;
//     *CADDR |= 1 << clock - 8 * Cshift; // clock HIGH
//     *CADDR &= ~(1 << clock - 8 * Cshift); // clock LOW
//   }