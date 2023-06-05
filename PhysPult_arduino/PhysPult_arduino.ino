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

const uint8_t pulseWidth = 10; // pulse width for work with registers (prevents from getting trash input)

const uint16_t idlemode_delay = 1050; // delay before going into idle mode
const uint16_t switches_delay = 30; // interval for updating switches state

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

ServoSmooth tm_servo;

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
  tm_servo.attach(tm_pwmpin);
  tm_servo.setSpeed(150);
  tm_servo.setAccel(0.25);

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
  static int pos = 0;
  static bool direction = 0;

  if(++pos > 180) {
    pos = 0;
    direction = !direction;
    tm_servo.setTargetDeg(180*direction);
  }
  
  tm_servo.tick();

  //--- OPERATING MODE ---
  if(Serial.available()) 
  {
    if(Serial.read() != '{') return;
  
    Serial.readBytesUntil('}', indicators, out_registerscount);

    idleModeTimer.start();

    updateSwitches(switches);
    updateIndicators(indicators);
    voltmeterShow(indicators[1]);

    Serial.write('{');
    Serial.write(switches, in_registerscount);
    Serial.write('}');
  }
  //--- OPERATING MODE ---

  //--- PULT LIGHTING ---
  if(pultLightingTimer.tick()) {
    if(switches[5] >> 4 & 1) pultLighting.setBrightness(0);
    else
      pultLighting.setBrightness(lighting_color[3]);  

    pultLighting.show();
  }
  //--- PULT LIGHTING ---

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
  //--- IDLE MODE ---
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

void updateSwitches(byte* output) {
  digitalWrite(in_latchpin, LOW);    
  delayMicroseconds(pulseWidth);
  digitalWrite(in_latchpin, HIGH);

  for(uint8_t i = 0; i < in_registerscount; i++) {
    for(uint8_t k = 7; k >= 0; k--)
    {
      if(digitalRead(in_datapin) == HIGH)
        bitSet(output[i], k);

      digitalWrite(in_clockpin, HIGH);
      delayMicroseconds(pulseWidth);
      digitalWrite(in_clockpin, LOW);
    }
  }           
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
  static uint8_t state = 0;

  if (stateTimer.tick())
  {
    state = (state + 1) % 4;

    digitalWrite(out_latchpin, 0);

    for(uint8_t i = 0; i < 3; i++)
      shiftOut(out_datapin, out_clockpin, LSBFIRST, 0);

    uint8_t random1 = random(0, 8),
            random2 = random(0, 8);
    while (random1 == 2 || random1 == 3) random1 = random(0, 8);

    shiftOut(out_datapin, out_clockpin, LSBFIRST, 1 << random1);
    shiftOut(out_datapin, out_clockpin, LSBFIRST, 1 << random2);

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