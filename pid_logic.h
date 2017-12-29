// -*- mode: c -*-
// Milliseconds

#define LOG_INTERVAL               2000
#define CHECK_TEMPERATURE_INTERVAL  100

// At 50 Hz a period is 20ms, so a half period is 10 ms = 10000 us
#define HALF_PERIOD_TIME_MICROS 10000

uint32_t on_percentage = 50;

uint32_t next_log_time = 0;              // Time to log next time (millis)
uint32_t zero_cross_count = 0;           // Number of zero crosses so far (wrapping is ok)
uint32_t turn_on_mark = 0;               // The zero-cross count when we turned the triac on
uint32_t next_turn_on = 0;		 // The time we want to turn on the triac on (microseconds)

uint32_t previous_log_time = 0;


void interrupt_rising() {
  // Triac turns off at zero crossing.
  // We want to turn it on after a short while (marked by next_turn_on)
  next_turn_on = micros() + (HALF_PERIOD_TIME_MICROS * on_percentage / 100);
  zero_cross_count++;
}

void setup_logic() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TRIAC_PIN, OUTPUT);
  pinMode(ZERO_CROSS_PIN, INPUT_PULLUP);
  attachInterrupt(ZERO_CROSS_PIN, interrupt_rising, RISING);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(TRIAC_PIN, LOW);
}

void loop_triac() {
  if (turn_on_mark == zero_cross_count) {
    // We have already turned it on this half-phase
    return;
  }

  uint32_t now = micros();

  if (now >= next_turn_on) {
    // Send a short spike to turn on the triac (it will stay on even after the pin is pulled low)
    digitalWrite(TRIAC_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(TRIAC_PIN, LOW);
    // Avoid turning it on again (no need to and may potentially give problems?)
    turn_on_mark = zero_cross_count;
  }
}

void loop_check_temperature() {
  static uint32_t next = 0;

  uint32_t now = millis();
  if (now < next)
    return;
  next = now + CHECK_TEMPERATURE_INTERVAL;

  // Do some magic PID stuff
}

void loop_log() {
  uint32_t now = millis();
  if (now < next_log_time)
    return;
  next_log_time = now + LOG_INTERVAL;
}

void loop_logic() {
  loop_triac();
  loop_check_temperature();
  loop_log();
}

