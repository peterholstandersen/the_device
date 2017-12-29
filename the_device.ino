// -*- mode: c -*-

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <stdint.h>

// ==== "Global" parameters and variables =====================================================

#define VERSION __FILE__ "   "  __DATE__ "   " __TIME__ 

#define GREEN_PIN               D4	// Power
#define YELLOW_PIN              D3	// WiFi
#define RED_PIN                 D5	// Heat / Error

#define TRIAC_PIN               D1
#define ZERO_CROSS_PIN          D2
#define ONE_WIRE_PIN  		D6
#define RELAY_PIN		D7

// Scale down the red led as it is much brigther than the others
#define RED_SCALE		12
#define RED_100			(1024 / RED_SCALE)
#define RED_75			( 768 / RED_SCALE)
#define RED_50			( 512 / RED_SCALE)
#define RED_25			( 256 / RED_SCALE)
#define RED_0			0

typedef enum {
  NO_ERROR = 0,
  ONE_WIRE_ERROR = 1,
  NOT_USED_YET = 2,
  NOT_USED_YET_EITHER = 3,
  NOT_USED_AT_ALL = 4
} Error;

Error error = NO_ERROR;


// ==== Temperature & PID settings  ===========================================================

#define ONE_WIRE_INTERVAL       1000

#define TARGET_TEMPERATURE      40.0f
#define TEMPERATURE_TOLERANCE   0.5f
#define TEMPERATURE_PRECISION   9

uint8_t deviceCount;
float *temperatures;
String *deviceId;


// ==== WiFi settings =========================================================================

#define WIFI_RETRY_DELAY        (10*60*1000) 	// Interval between trying to reconnect to WiFi
#define WIFI_INTERVAL		(60*1000)    	// Interval between uploading and checking for updates
#define UPDATE_URL		"/cgi-bin/update.php"
#define LOG_URL			"/cgi-bin/log_message.py"
#define SSID			"CableBox-7A3D"
#define HOST                    "192.168.0.17"	// ThinkCentre. Fixed IP.
#define PORT			80

const String log_temperature_url = "/cgi-bin/log_temperature.py";

const char *password = "farerdenbedste";


// ==== Code ==================================================================================

void log_message(String msg);
void log_message_serial(String msg);

#include "error.h"    // Must be first
#include "one_wire.h"
#include "wifi.h"
// #include "relay_logic.h"
#include "pid_logic.h"

void log_message(String msg)
{
  log_message_serial(msg);
  if (WiFi.status() == WL_CONNECTED) {
    msg.replace(' ', '+');
    msg.replace('\n', '+');
    String url = LOG_URL "?id=the_device&msg=" + msg;
    do_request(url);
  }
}

void log_message_serial(String msg)
{
  // Serial.print(msg);
}


void setup()
{
  Serial.begin(115200);
  
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  
  log_message_serial("The Device 1.0\n");
  log_message_serial(VERSION "\n");

  setup_error();
  setup_wifi();

  log_message("The Device 1.0\n");
  log_message(VERSION "\n");

  setup_one_wire();
  setup_logic();

  if (error == NO_ERROR)
    digitalWrite(GREEN_PIN, HIGH);
}


void loop()
{
  loop_error();      // Update LEDs upon error
  loop_one_wire();
  loop_wifi();
  loop_logic();
}
