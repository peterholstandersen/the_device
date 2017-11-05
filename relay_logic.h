// Dont start relay right away
static const long initial_delay = 5000;
static const long relay_interval = 1000;

static long allowed_change_time = 5000;
static long relay_keep_state = 10000;
static long relay_next = -1;

void setup_logic()
{
  if (error != NO_ERROR)
    return;
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}

float get_actual_temperature() {
  // For now, we only have one probe
  return temperatures[0];
}

void loop_logic() {
  if (error != NO_ERROR)
    return;
  
  long now = millis();
  if (now < relay_next)
    return;
  relay_next = now + relay_interval;

  float actual_temperature = get_actual_temperature();

  int desired_state;
  int actual_state = digitalRead(RELAY_PIN);

  if (actual_temperature > TARGET_TEMPERATURE + TEMPERATURE_TOLERANCE)
    desired_state = LOW;

  if (actual_temperature < TARGET_TEMPERATURE - TEMPERATURE_TOLERANCE)
    desired_state = HIGH;

  if (actual_state == desired_state)
    return;

  if (now < allowed_change_time)
    return;

  if (desired_state == LOW) {
    analogWrite(RED_PIN, RED_0);
    log_message("Relay off\n");
  }
  else {
    analogWrite(RED_PIN, RED_100);
    log_message("Relay on\n");
  }

  digitalWrite(RELAY_PIN, desired_state);

  // Not allowed to change the relay state for a while
  allowed_change_time = now + relay_keep_state;
}
