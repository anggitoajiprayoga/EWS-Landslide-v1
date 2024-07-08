#include <TinyGPSPlus.h>
#define TINY_GSM_MODEM_SIM7600
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "I2Cdev.h"
#include "Wire.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <Wire.h>
#include <RTClib.h>
#include "SPIFFS.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_task_wdt.h>
#include <movingAvg.h>

#define SDA2 17
#define SCL2 16

#define SD_CS 5

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1
#define INDICATOR_SIZE 6

#define p_kalibrasi 12
#define p_default 13
#define p_sv 39
#define p_sv1 39

#define devAddr 0x69
#define devAddr1 0x68

#define ENCODER_PIN_A 4
#define ENCODER_PIN_B 27
#define LED 2

#define RXPin 33
#define TXPin 32
#define GPSBaud 9600

#define SerialAT Serial1
#define PROTOCOL SERIAL_8N1
#define rx 25
#define tx 26
#define RST 14
#define WDT_TIMEOUT 60

#define Diameter 25 //in mm
#define CPR 2000 //Counts per revolution of each phase.

String version = "Firm v1.0 - EWS Landslide v1";
String firm_ver = "v1.0";
String xfirm_ver;
String type_board = "EWS Prototype";
String type = "EWS";
String sn_str = "1234";

const char apn[]      = "Internet";
const char gprsUser[] = "";
const char gprsPass[] = "";
const int port_ota = 80;

String http_username;
String http_password;
String ssid_wifi_client;
String pass_wifi_client;
String ssid_wifi_ap;
String pass_wifi_ap;
String freesp, usedstr, totalstr;

String webpage = "";
bool shouldReboot = false;
String listFiles(bool ishtml = false);

const String default_ssid = "";
const String default_wifipassword = "SS6";
const String default_httpuser = "admin";
const String default_httppassword = "admin";
const int default_webserverporthttp = 80;

String ssid;
String wifipassword;
String httpuser;
String httppassword;
int webserverporthttp;

uint64_t sizeTotal, sizeKB, totalKB, usedKB, usedB, spaceKB;
uint32_t usedByte;

float offset = 93; //145
float factor = (3.1415 * Diameter) / (CPR);
float distance_mm;
volatile long encoderPosition = 0;

float angVal, angVal1;
byte mpuIntStatus, mpuIntStatus1;
byte devStatus, devStatus1;
int packetSize, packetSize1;
int fifoCount;
byte fifoBuffer[64];
byte fifoBuffer1[64];

int Year;
byte Month;
byte Day;
byte Hour;
byte Minute;
byte Second;

String no_simcard, imei;
String condition_state;
String dates, times;
String dateTime, timeZone = "+8:00", lat, lng, alt, spd, hdg, topic_sub;
String mqtt_state, rssi, ind;
String value_year, value_month, value_day, value_hour, value_minute, value_second;
long prev_mqtt, prev_sermon;

String cn, sn, en_lte, en_mqtt, broker, username, password, interval_mqtt, server_ota, resource_ota;
String _en_lte, _en_mqtt, _broker, _username, _password, _interval_mqtt, _server_ota, _resource_ota, _setpointX0;
String from_safe, to_safe, from_alert, to_alert, from_caution, to_caution, from_danger, to_danger;

bool encoderFlag;
bool en_update;
bool state_sdcard;
bool user_login;
bool eror_gyro1, eror_gyro2, eror_lcd, eror_gps, eror_sdcard, eror_rtc;

float Z0;
float Z1;
float rope_default = 1000;
float dist_slide;
float setpoint_X0;
float dist_enco;
float long_Y1;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
WiFiServer serverAP(80);
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
TwoWire Wire2 = TwoWire(1);
RTC_DS1307 rtc;
movingAvg avg_battery(10);
movingAvg avg_pv(10);
movingAvg avg_gyro(10);
movingAvg avg_gyro1(10);
movingAvg avg_Z0(10);
movingAvg avg_Z1(10);
SoftwareSerial serial_gps(RXPin, TXPin);
TinyGPSPlus gps;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU6050 mpu(devAddr);
MPU6050 mpu1(devAddr1);
Quaternion q;

String payload() {
  String json_data = "";
  DynamicJsonDocument buffer(1024);
  JsonObject obj            = buffer.to<JsonObject>();
  obj["cn"]                 = cn;
  obj["sn"]                 = sn;
  obj["dateTime"]           = dateTime;
  obj["timeZone"]           = timeZone;
  obj["displacement_z1_z0"]   = atof(String(dist_slide, 1).c_str());
  obj["inclination_angle_a0"] = atof(String(setpoint_X0, 1).c_str());
  obj["inclination_angle_a1"] = atof(String(angVal, 1).c_str());
  obj["slope_length_y0"]      = atof(String(rope_default, 1).c_str());
  obj["slope_length_y1"]      = atof(String(long_Y1, 1).c_str());
  obj["horizontal_length_z0"] = atof(String(Z0, 1).c_str());
  obj["horizontal_length_z1"] = atof(String(Z1, 1).c_str());
  obj["rotary_encorder"]      = atof(String(dist_enco, 1).c_str());
  obj["gyroscope_y"]          = atof(String(angVal, 1).c_str());
  obj["gyroscope_x"]          = atof(String(angVal1, 1).c_str());
  obj["condition_state"]      = condition_state;
  obj["battery_voltage"]      = atof(String(battery_voltage(), 1).c_str());
  obj["pv_voltage"]           = atof(String(pv_voltage(), 1).c_str());
  obj["rssi"]                 = rssi.toInt();

  JsonObject dataSupport    = obj.createNestedObject("data");
  dataSupport["dateTime"]   = dateTime;
  dataSupport["timeZone"]   = timeZone;
  dataSupport["lat"]        = roundToPrecision(atof(lat.c_str()), 7);
  dataSupport["lng"]        = roundToPrecision(atof(lng.c_str()), 7);
  dataSupport["alt"]        = atof(alt.c_str());
  dataSupport["spd"]        = atof(spd.c_str());
  dataSupport["hdg"]        = atof(hdg.c_str());
  serializeJsonPretty(obj, json_data);
  return json_data;
}

String payload_web() {
  String json_data = "";
  DynamicJsonDocument buffer(1024);
  JsonObject obj            = buffer.to<JsonObject>();
  obj["cn"]                 = cn;
  obj["sn"]                 = sn;
  obj["dateTime"]           = dateTime;
  obj["timeZone"]           = timeZone;
  obj["displacement_z1_z0"]   = atof(String(dist_slide, 1).c_str());
  obj["inclination_angle_a0"] = atof(String(setpoint_X0, 1).c_str());
  obj["inclination_angle_a1"] = atof(String(angVal, 1).c_str());
  obj["slope_length_y0"]      = atof(String(rope_default, 1).c_str());
  obj["slope_length_y1"]      = atof(String(long_Y1, 1).c_str());
  obj["horizontal_length_z0"] = atof(String(Z0, 1).c_str());
  obj["horizontal_length_z1"] = atof(String(Z1, 1).c_str());
  obj["rotary_encorder"]      = atof(String(dist_enco, 1).c_str());
  obj["gyroscope1"]          = atof(String(angVal, 1).c_str());
  obj["gyroscope2"]          = atof(String(angVal1, 1).c_str());
  obj["condition_state"]      = condition_state;
  obj["battery_voltage"]      = atof(String(battery_voltage(), 1).c_str());
  obj["pv_voltage"]           = atof(String(pv_voltage(), 1).c_str());
  obj["rssi"]                 = rssi.toInt();
  obj["lat"]                 = roundToPrecision(atof(lat.c_str()), 7);
  obj["lng"]                 = roundToPrecision(atof(lng.c_str()), 7);
  obj["alt"]                 = atof(alt.c_str());
  obj["ind"]                 = ind;
  serializeJsonPretty(obj, json_data);
  return json_data;
}

String liveLocations() {
  String livelocation = "";
  DynamicJsonDocument buffer(1024);
  JsonObject obj  = buffer.to<JsonObject>();
  obj["sn"]       = sn;
  obj["cn"]       = cn;
  obj["type"]     = type;
  obj["dateTime"] = dateTime;
  obj["timeZone"] = timeZone;
  obj["lat"]      = roundToPrecision(atof(lat.c_str()), 7);
  obj["lng"]      = roundToPrecision(atof(lng.c_str()), 7);
  obj["alt"]      = atof(alt.c_str());
  obj["spd"]      = atof(spd.c_str());
  obj["hdg"]      = atof(hdg.c_str());
  obj["rssi"]     = rssi.toInt();
  obj["firm"]     = firm_ver;
  serializeJsonPretty(obj, livelocation);
  return livelocation;
}

String config() {
  String device_config = "";
  //  DynamicJsonDocument buffer(1024);
  StaticJsonDocument<1024> buffer;
  JsonObject obj         = buffer.to<JsonObject>();
  obj["cn"]              = cn;
  obj["sn"]              = sn;
  obj["date"]            = "";
  obj["time"]            = "";
  obj["slope_length_y0"] = rope_default;
  obj["from_safe"]       = from_safe;
  obj["to_safe"]         = to_safe;
  obj["from_alert"]      = from_alert;
  obj["to_alert"]        = to_alert;
  obj["from_caution"]    = from_caution;
  obj["to_caution"]      = to_caution;
  obj["from_danger"]     = from_danger;
  obj["to_danger"]       = to_danger;
  obj["en_lte"]          = en_lte;
  obj["en_mqtt"]         = en_mqtt;
  obj["broker"]          = broker;
  obj["username"]        = username;
  obj["password"]        = password;
  obj["interval_mqtt"]   = interval_mqtt;
  obj["server_ota"]      = server_ota;
  obj["resource_ota"]    = resource_ota;
  obj["imei"]            = imei;
  obj["no_simcard"]      = no_simcard;
  serializeJsonPretty(obj, device_config);
  return device_config;
}

String state_eror() {
  String state_eror = "";
  StaticJsonDocument<1024> buffer;
  JsonObject obj     = buffer.to<JsonObject>();
  obj["eror_gyro1"]  = eror_gyro1;
  obj["eror_gyro2"]  = eror_gyro2;
  obj["eror_lcd"]    = eror_lcd;
  obj["eror_gps"]    = eror_gps;
  obj["eror_sdcard"] = eror_sdcard;
  obj["eror_rtc"]    = eror_rtc;

  serializeJsonPretty(obj, state_eror);
  return state_eror;
}

void notifyClients(String payload) {
  ws.textAll(payload);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.println((char*)data);
    websocket_parse(String((char*)data));
    String get_payload = payload_web();
    Serial.println("//web_socket_payload_receive//");
    Serial.println(get_payload);
    notifyClients(get_payload);

    if (strcmp((char*)data, "gyro") == 0) {
      setCalibration();
      Serial.println("Kalibrasi All Gyroscope - Zero Point");
    }
    if (strcmp((char*)data, "encorder") == 0) {
      distance_mm = 0;
      encoderPosition = 0;
      writeFile(SPIFFS, "/encoderPosition.txt", String(encoderPosition).c_str());
      Serial.println("Kalibrasi Rotary Encorder - Zero Point");
    }
    if (strcmp((char*)data, "get alpha") == 0) {
      setpoint_X0 = angVal;
      writeFile(SPIFFS, "/setpoint_X0.txt", String(setpoint_X0).c_str());
      Serial.println("Get Angle Alpha from Gyroscope - 90");
    }
  }
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      user_login = true;
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      user_login = false;
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      Serial.printf("WebSocket client #%u pong received\n", client->id());
      break;
    case WS_EVT_ERROR:
      Serial.println("WebSocket error occurred. Restarting ESP32...");
      user_login = false;
      delay(2000);
      ESP.restart();
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);
  setupGps();
  setup_encorder();
  SPIFFS.begin();
  read_setting();
  setup_webserver();
  setup_wifi();
  initWebSocket();
  setupmpu();
  setup_display();
  setup_main();
  setup_rtc();
  setup_sd_card();
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

void loop() {
  loop_gps();
  loopmpu();
  loop_encorder();
  loop_rtc();
  loop_main();
  event();
  loop_display();
  loop_wifi();
  loop_logging();
  serial_monitor();
  esp_task_wdt_reset();
}
