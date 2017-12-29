// Read temperatures once per second

// Error if there is more than one probe

static OneWire oneWire(ONE_WIRE_PIN);
static DallasTemperature sensors(&oneWire);
static DeviceAddress *deviceAddress;

float get_actual_temperature() {
  float actual_temperature = 0.0;

  // Just get the highest temperature
  for (int i = 0; i < deviceCount; i++) {
    if (temperatures[i] > actual_temperature)
      actual_temperature = temperatures[i];
  }
  return actual_temperature;
}

void setup_one_wire() {
  if (error != NO_ERROR)
    return;
  
  pinMode(ONE_WIRE_PIN, INPUT_PULLUP);

  sensors.begin();
  deviceCount = sensors.getDeviceCount();

  log_message("Found " + String(deviceCount) + " one-wire device(s)\n");
  
  if (deviceCount != 1) {
    set_error(ONE_WIRE_ERROR);
    return;
  }
  
  deviceAddress = new DeviceAddress[deviceCount];
  temperatures = new float[deviceCount];
  deviceId = new String[deviceCount];
  
  for (uint8_t i = 0; i < deviceCount; i++) {
    (void) sensors.getAddress(deviceAddress[i], i);

    deviceId[i] = "0x";
    for (uint8_t j = 0; j < 8; j++)
      deviceId[i] = deviceId[i] + String(deviceAddress[i][j]);
    
    sensors.setResolution(deviceAddress[i], TEMPERATURE_PRECISION);

    log_message("deviceId[" + String(i) + "] = " + String(deviceId[i]) + "\n");
  }
  
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
}

static long one_wire_next = -1;
static const long one_wire_interval = 1000;

void loop_one_wire() {
  if (error != NO_ERROR)
    return;
  
  long now = millis();
  if (now < one_wire_next)
    return;

  one_wire_next = now + one_wire_interval;

  for (uint8_t i = 0; i < deviceCount; i++) {
    float reading = sensors.getTempC(deviceAddress[i]);

    // log_message(String(temperatures[i], 1) + "  (target " + String(TARGET_TEMPERATURE, 1) + ")\n");

    // Only keep most sensible readings
    if (-20.0 <= reading && reading <= 110.0) {
      temperatures[i] = reading;
    }
    else {
      log_message("Reading disregarded as an error");
    }
  }
  
  // Requests temperatures (will take up to 750ms to be ready).
  // However, the call below is not blocking, but relies on the this function not being
  // called more often than every 750ms

  sensors.requestTemperatures();
}


