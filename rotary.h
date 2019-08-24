#include <avr/io.h>
#include <avr/interrupt.h>

#define ROTPORT     PORTC
#define ROTDDR      DDRC
#define ROTPIN      PINC

#define ROTPA       PC1
#define ROTPB       PC2
#define ROTPBUTTON	PC0

#define ROTA !((1<<ROTPA)&ROTPIN)
#define ROTB !((1<<ROTPB)&ROTPIN)
#define ROTCLICK !((1<<ROTPBUTTON)&ROTPIN)

void RotaryInit(void);
void RotaryCheckStatus(void);
int16_t RotaryGetStatus(void);
uint8_t RotaryGetStatusB(void);
uint8_t RotaryLeftPosition (void);
uint8_t RotaryRightPosition (void);
void RotaryResetStatus(int16_t rst);
void RotaryResetPosition();
/////////////////////////////////////
static int16_t rotarystatus   = 0;
static uint8_t wait           = 0;
static int8_t  rotarystatusB  = 0;
static int8_t  Left           = 0;
static int8_t  Right          = 0;
void Timer_Start(void) {
  TCNT2   = 0;
  TCCR2B |= (1 << WGM22);     // Configure timer 2 for CTC mode
  TCCR2B |= (1 << CS22);      // Start timer at Fcpu/64
  TIMSK2 |= (1 << OCIE2A);    // Enable CTC interrupt
  OCR2A   = 100;              // Set CTC compare value with a prescaler of 64
  sei();                      // TÜM TIMER'LAR START
}
ISR(TIMER2_COMPA_vect) {
  RotaryCheckStatus();
}
void RotaryInit(void) {
  ROTDDR &= ~((1 << ROTPA) | (1 << ROTPB) | (1 << ROTPBUTTON));
  DDRD &= ~(1 << ROTPBUTTON);
  PORTD |= (1 << ROTPBUTTON);
  ROTPORT |= (1 << ROTPA) | (1 << ROTPB) | (1 << ROTPBUTTON);
  Timer_Start();
}
void RotaryCheckStatus(void) {
  if (ROTA & (!wait)) wait = 1;
  if (ROTB & ROTA & (wait))
  {
    rotarystatus++;
    wait = 2;
    Right = true;
  }
  else if (ROTA & (!ROTB) & wait)
  {
    rotarystatus--;
    wait = 2;
    Left = true;
  }
  if ((!ROTA) & !(ROTB) & (wait == 2))
    wait = 0;
  if (ROTCLICK)
    rotarystatusB = 1;
  else
    rotarystatusB = 0;
}
int16_t RotaryGetStatus(void) {
  return rotarystatus;
}
uint8_t RotaryGetStatusB(void) {
  return rotarystatusB;
}
void RotaryResetStatus(int16_t rst) {
  rotarystatus = rst;
}
uint8_t RotaryLeftPosition(void) {
  return Left;
}
uint8_t RotaryRightPosition(void) {
  return Right;
}
void RotaryResetPosition() {
  Right = Left = false;
}
