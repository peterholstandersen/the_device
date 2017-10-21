void setup_relay()
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

static long allowed_change_time = 0;
static long relay_next = -1;
static const long relay_interval = 1000;

void loop_relay() {
  if (error != NO_ERROR)
    return;
  
  long now = millis();
  if (now < relay_next)
    return;
  relay_next = now + relay_interval;

  Serial.println("loop_relay");

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

  if (desired_state == LOW)
    Serial.println("Relay off");
  else
    Serial.println("Relay on");

  digitalWrite(RELAY_PIN, desired_state);

  // Not allowed to change the relay state for the next 30 seconds
  allowed_change_time = now + 30 * 1000;
}


