#include <microLED.h>
#include <ServoSmooth.h>
#include <TimerMs.h>

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

const uint32_t BaudRate = 38400;

const uint16_t idlemode_delay = 1050; // delay before going into idle mode
const uint16_t switches_delay = 30; // interval for updating switches state

const uint8_t out_datapin = 2; // 71hc595 registers
const uint8_t out_clockpin = 4; // 71hc595 registers clock pin
const uint8_t out_latchpin = 7; // 71hc595 registers latch pin
const uint8_t out_registerscount = 10; // 71hc595 registers count

const uint8_t in_datapin = 8; // 71hc165 registers
const uint8_t in_clockpin = 12; // 71hc165 registers clock pin
const uint8_t in_latchpin = 13; // 71hc165 registers latch pin 
const uint8_t in_registerscount = 6; // 71hc165 registers count

uint8_t indicators[out_registerscount];
uint8_t switches[in_registerscount];

const uint8_t voltmeter_pwmpin = 5;

const uint8_t tm_pwmpin = 9;
const uint8_t nm_pwmpin = 10;
const uint8_t tc_pwmpin = 11;

ServoSmooth tm_servo;
ServoSmooth nm_servo;
ServoSmooth tc_servo;

const uint8_t lighting_datapin = 3; // pult lighting
const uint8_t lighting_ledcount = 8;
// uint8_t lighting_color[4] = { 30, 200, 255, 150 }; // HSV + brightness (FastLED)
uint8_t lighting_color[4] = { 25, 200, 255, 50 }; // HSV + brightness (MicroLED)

microLED<lighting_ledcount, lighting_datapin, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER, SAVE_MILLIS> pultLighting;

TimerMs idleModeTimer(idlemode_delay, 1, 1);
TimerMs switchesTimer(switches_delay, 1, 0);
TimerMs pultLightingTimer(50, 1, 0); // must be >40 mks

void setup()
{
  pinMode(out_datapin, OUTPUT);
  pinMode(out_clockpin, OUTPUT);
  pinMode(out_latchpin, OUTPUT);

  pinMode(in_datapin, INPUT);
  pinMode(in_clockpin, OUTPUT);
  pinMode(in_latchpin, OUTPUT);

  for(uint8_t i = 0; i < out_registerscount; i++) indicators[i] = 0;
  for(uint8_t i = 0; i < in_registerscount; i++) switches[i] = 0;

  pinMode(voltmeter_pwmpin, OUTPUT);
  voltmeterShow(75);

  pinMode(tm_pwmpin, OUTPUT);
  pinMode(nm_pwmpin, OUTPUT);
  pinMode(tc_pwmpin, OUTPUT);

  tm_servo.setSpeed(70);
  nm_servo.setSpeed(70);
  tc_servo.setSpeed(70);

  tm_servo.setAccel(0.14);
  nm_servo.setAccel(0.14);
  tc_servo.setAccel(0.14);

  tm_servo.write(0);
  nm_servo.write(0);
  tc_servo.write(0);

  tm_servo.attach(tm_pwmpin);
  nm_servo.attach(nm_pwmpin);
  tc_servo.attach(tc_pwmpin);

  pinMode(lighting_datapin, OUTPUT);

  pultLighting.setBrightness(0);
  // pultLighting.setBrightness(lighting_color[3]);
  pultLighting.fill(mHSV(lighting_color[0], lighting_color[1], lighting_color[2]));
  
  pultLighting.show();

  Serial.begin(BaudRate);
  Serial.setTimeout(15);

  updateIndicatorsOn();
  delay(500);
  updateIndicatorsOff();

  while(Serial.available()) Serial.read();
}

void loop()
{ 
  static TimerMs sweep(4000, 1, 0);
  static TimerMs sweep2(3000, 1, 0);
  static bool direction = 0;
  static bool direction2 = 0;

  if(sweep.tick()) {
    direction = !direction;
    tm_servo.setTargetDeg(180*direction);
  }
  if(sweep2.tick()) {
    direction2 = !direction2;
    nm_servo.setTargetDeg(180*direction2);
  }
  
  // tm_servo.setTargetDeg(0);
  // nm_servo.setTargetDeg(0);

  tm_servo.tick();
  nm_servo.tick();
  tc_servo.tick();

  //--- OPERATING MODE ---
  if(Serial.available()) {
    if(Serial.read() != '{') return;
  
    Serial.readBytesUntil('}', indicators, out_registerscount);

    idleModeTimer.start();

    updateSwitches(switches);
    updateIndicators(indicators);
    
    voltmeterShow(indicators[1]);

    tm_servo.setTargetDeg(map(indicators[2], 0, 155, 0, 180));
    nm_servo.setTargetDeg(map(indicators[3], 0, 155, 0, 180));
    tc_servo.setTargetDeg(map(indicators[4], 0, 55, 0, 180));

    Serial.write('{');
    Serial.write(switches, in_registerscount);
    Serial.write('}');
  }

  //--- PULT LIGHTING ---
  if(pultLightingTimer.tick()) {
    if(switches[5] >> 4 & 1) pultLighting.setBrightness(0);
    else
      pultLighting.setBrightness(lighting_color[3]);  

    pultLighting.show();
  }

  //--- IDLE MODE ---
  if(idleModeTimer.elapsed()) {

    updateIndicatorsIdle();

    if(switchesTimer.tick()) {
      updateSwitches(switches);

      if(!(switches[0] >> 3 & 1)) {
        if(lighting_color[3] < 254)
          lighting_color[3] += 2;
      }
      if(!(switches[0] >> 2 & 1)) {
        if(lighting_color[3] > 1)
          lighting_color[3] -= 2;
      }
    }
  }
}

void voltmeterShow(uint8_t voltage) {
  voltage %= 151; // Overflow protection
  analogWrite(voltmeter_pwmpin, map(voltage, 0, 150, 0, 255));
}

void updateIndicators(byte* command)
{
  char speed = command[0];
  uint8_t secondSpeedByte = leftdigit[speed / 10][0] | rightdigit[speed % 10][0] | command[5];
  uint8_t firstSpeedByte = leftdigit[speed / 10][1] | rightdigit[speed % 10][1]; 

  digitalWrite(out_latchpin, 0);
  //*LADDR &= ~(1 << latch - 8 * Lshift); // latch LOW
  //delayMicroseconds(pulseWidth);

  shiftOut(out_datapin, out_clockpin, LSBFIRST, command[8]); // Регистр 5
  shiftOut(out_datapin, out_clockpin, LSBFIRST, command[6]); // Регистр 4
  shiftOut(out_datapin, out_clockpin, LSBFIRST, command[7]); // Регистр 3

  shiftOut(out_datapin, out_clockpin, LSBFIRST, secondSpeedByte); // Регистр 2
  shiftOut(out_datapin, out_clockpin, LSBFIRST, firstSpeedByte); // Регистр 1 

  //delayMicroseconds(pulseWidth);
  //*LADDR |= 1 << latch - 8 * Lshift; // latch HIGH
  digitalWrite(out_latchpin, 1);
}

uint8_t read165(uint8_t data, uint8_t clock)
{
  uint8_t output = 0;

  // The first one that is read is the highest bit (input D7 of the 74HC165).
  for(int i = 7; i >= 0; i--)
  {
    if(digitalRead(data) == HIGH)
      bitSet(output, i);

    digitalWrite(clock, HIGH);
    delayMicroseconds(10);
    digitalWrite(clock, LOW);
  }

  return output;
}

void updateSwitches(byte* output) {
  digitalWrite(in_latchpin, LOW);    
  delayMicroseconds(10);
  digitalWrite(in_latchpin, HIGH);

  for(uint8_t i = 0; i < in_registerscount; i++)
    output[i] = read165(in_datapin, in_clockpin);           
}

void updateIndicatorsOn()
{
  uint8_t on[5] = {88, 255, 255, 255, 255},
  updateIndicators(on);
}

void updateIndicatorsOff()
{
  uint8_t off[5] = {0, 0, 0, 0, 0};
  updateIndicators(off);
}

void updateIndicatorsIdle()
{
  static TimerMs stateTimer(250, 1, 0);

  if (stateTimer.tick())
  {
    digitalWrite(out_latchpin, 0);

    for(uint8_t i = 0; i < 3; i++)
      shiftOut(out_datapin, out_clockpin, LSBFIRST, 0);

    uint8_t rand = random(0, 8);
    while (rand == 2 || rand == 3) rand = random(0, 8);

    shiftOut(out_datapin, out_clockpin, LSBFIRST, 1 << rand);
    shiftOut(out_datapin, out_clockpin, LSBFIRST, 1 << random(0, 8));

    digitalWrite(out_latchpin, 1);

    voltmeterShow(69);
  }
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