void setup_wifi();
void update_wifi_led();
bool connect_wifi(bool force);
void check_update();
void log_temperature();

static long wifi_next;

static long next_wifi_retry = 0;

// ================================================================================
// setup_wifi
// ================================================================================
void setup_wifi()
{
  if (error != NO_ERROR)
    return;
  
  log_message_serial_only("Connecting to " SSID "\n");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, password);

  connect_wifi(true);
}

// ================================================================================
// loop_wifi
// ================================================================================
void loop_wifi() {
  if (error != NO_ERROR)
    return;

  update_wifi_led();
  
  long now = millis();
  if (now < wifi_next)
    return;
  wifi_next = now + WIFI_INTERVAL;

  if (!connect_wifi(false))
    return;

  log_temperature();
  check_update();
}


// ================================================================================
// update_wifi_led
// ================================================================================
void update_wifi_led() {
  digitalWrite(YELLOW_PIN, (WiFi.status() == WL_CONNECTED) ? HIGH : LOW);
}

// ================================================================================
// connect_wifi
// Connect to WiFi if not already connected
// ================================================================================
bool connect_wifi(bool force) {

  if (WiFi.status() == WL_CONNECTED)
    return true;

  // Not connected: check if it is time try again
  long now = millis();
  if (!force && next_wifi_retry != 0 && now < next_wifi_retry) {
    log_message_serial_only("Not time to reconnect yet\n");
    return false;
  }

  bool led_on = true;
  long timeout = now + 20000;
  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
    led_on = !led_on;
    digitalWrite(YELLOW_PIN, led_on ?HIGH :LOW);
    delay(500);
    log_message_serial_only(".");
  }
  log_message_serial_only("\n");

  update_wifi_led();
  
  if (WiFi.status() == WL_CONNECTED) {
    log_message("WiFi connected.\n");
    return true;
  }
  else {
    log_message_serial_only("Unable to connect to WiFi\n");
    next_wifi_retry = millis() + WIFI_RETRY_DELAY;
    return false;
  }
}

// ================================================================================
// check_update
// ================================================================================
void check_update()
{
  log_message("Check update\n");

  t_httpUpdate_return ret = ESPhttpUpdate.update(HOST, PORT, UPDATE_URL, VERSION);

  switch(ret) {
  case HTTP_UPDATE_FAILED:
    log_message("Update failed\n");
    break;
  case HTTP_UPDATE_NO_UPDATES:
    log_message("No update available\n");
    break;
  case HTTP_UPDATE_OK:
    // Never reaches this point as the device reboots before
    break;
  }
}

bool do_request(String url)
{
  log_message_serial_only("Connecting to " HOST "\n");

  WiFiClient client;

  if (!client.connect(HOST, PORT)) {
    log_message_serial_only("Connection failed");
    return false;
  }
  
  log_message_serial_only("Requesting URL: " + String(url) + "\n");
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n"
	       "Host: " HOST "\r\n"
	       "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      log_message_serial_only("HTTP request timeout\n");
      // TODO: cleanup
      client.stop();
      return false;
    }
  }

  log_message_serial_only("Closing connection\n");
  client.stop();

  return true;
}


// ================================================================================
// log_temperature
// ================================================================================
void log_temperature()
{
  for (int i = 0; i < deviceCount; i++) {
    digitalWrite(YELLOW_PIN, LOW);
    delay(250);
    digitalWrite(YELLOW_PIN, HIGH);
    
    String url = log_temperature_url + "?id=" + deviceId[i] + "&temperature=" + String(temperatures[i], 1);
    
    if (!do_request(url))
      return;
  }
}

