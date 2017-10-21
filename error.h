static long next_red_on = 0;
static long next_red_off = 0;
static const long red_on = 100;
static const long red_off = 500;

void set_error(Error cause)
{
  Serial.print("Error: " + String(cause) + "\n");
  
  int green = LOW;
  int yellow = LOW;
  
  error = cause;

  switch (cause) {
  case ONE_WIRE_ERROR:
    green = HIGH;
    yellow = LOW;
  }

  digitalWrite(GREEN_PIN, green);
  digitalWrite(YELLOW_PIN, yellow);
  digitalWrite(RED_PIN, HIGH);

  next_red_off = millis() + red_on;
  next_red_on = next_red_off + red_off;
}

bool setup_error()
{
  // Do nothing
  return true;
}

// Blink red led if something has failed
void loop_error()
{
  if (error == NO_ERROR)
    return;
  
  long now = millis();
  if (now >= next_red_on) {
    digitalWrite(RED_PIN, HIGH);
    next_red_off = now + red_on;
    next_red_on = next_red_off + red_off;
  }
  else if (now >= next_red_off) {
    digitalWrite(RED_PIN, LOW);
    next_red_on = now + red_off;
    next_red_off = next_red_on + red_on;
  }
}

