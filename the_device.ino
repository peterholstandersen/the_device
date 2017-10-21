// -*- mode: c -*-

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>

// ==== "Global" parameters and variables =====================================================

#define TEST

#define GREEN_PIN               D4	// Power
#define YELLOW_PIN              D3	// WiFi
#define RED_PIN                 D5	// Heat / Error
#define ONE_WIRE_PIN  		D6
#define RELAY_PIN		D7

#define RED_100			(1024 / 12)
#define RED_75			( 768 / 12)
#define RED_50			( 512 / 12)
#define RED_25			( 256 / 12)

// TODO
// #define ZERO_CROSS_PIN
// #define PID_PIN

#define TARGET_TEMPERATURE      78.0f
#define TEMPERATURE_TOLERANCE   0.5f
#define TEMPERATURE_PRECISION   12

#define WIFI_RETRY_DELAY        (10*60*1000) // 10 minutes
// #define WIFI_UPLOAD_INTERVAL	(60*1000)    // 1 minute
#define WIFI_UPLOAD_INTERVAL	(10*1000)    // 10 seconds (for testing)

const char *ssid     = "CableBox-7A3D";
const char *password = "farerdenbedste";
const char *host     = "192.168.0.10";		// X301. Fixed IP.

String url_template = "/cgi-bin/log_temperature.py";

uint8_t deviceCount;
float *temperatures;
String *deviceId;

typedef enum {
  NO_ERROR = 0,
  ONE_WIRE_ERROR = 1,
  NOT_USED_YET = 2,
  NOT_USED_YET_EITHER = 3,
  NOT_USED_AT_ALL = 4
} Error;

Error error = NO_ERROR;

#include "error.h"    // Must be first
#include "one_wire.h"
#include "wifi.h"
#include "relay.h"

void setup()
{
  Serial.begin(115200);
  
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  
  Serial.println("The Device 1.0");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.print(" ");
  Serial.print(__FILE__);
  Serial.print("\n");

  setup_error();
  setup_one_wire();
  setup_wifi();
  setup_relay();

  if (error == NO_ERROR)
    digitalWrite(GREEN_PIN, HIGH);
}


void loop()
{
  loop_error();
  loop_one_wire();
  loop_wifi();
  loop_relay();
}
