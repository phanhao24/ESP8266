#include <ESP8266WiFi.h>  //wifi esp
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DNSServer.h>         //esp
#include <ESP8266WebServer.h>  //esp
#include <WiFiManager.h>
#include <SPI.h>
#include <SocketIoClient.h>  //socket io
#include <ArduinoJson.h>     //data Json
#include "DHT.h"             //dht11
#include <stdint.h>          //sensor GM
#include <string.h>          //sensor GM
#include <time.h>            //realtime from internet
#include <WiFiUdp.h>         //realtime
#include <NTPClient.h>       //realtime
#include <ESP8266mDNS.h>     // Booting Code From wifi
#include <ArduinoOTA.h>      // Booting Code From wifi
#include <SD.h>              // sd card
#include <EEPROM.h>          //EEPROM 512 byte
#include <Wire.h>
#include <hd44780.h>                        // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h>  // i2c expander i/o class header
#include <LiquidCrystal_I2C.h>
#include "Secrets.h"
#include <math.h>

#define DHT11Pin D5  //D4,D3,D5
#define DHTType DHT11
#define PIN_TICK D6  //D5,D2
#define AWS_IOT_PUBLISH_TOPIC "tram3/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "tram3/sub"


DHT HT(DHT11Pin, DHTType);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP ntpUDP;
WiFiClientSecure net;
NTPClient timeClient(ntpUDP, "vn.pool.ntp.org");


static float humi = 0;
static float tempC = 0;
static long timeTick = 0;
const char* statusStation = "Tram3";
int addr = 0;
static volatile unsigned long counts = 0;
static float uSv;
static float cps = 0;
static unsigned long int timeCount_present;
static unsigned long int count_present;
static unsigned long int timeCount_prev = 0;
static unsigned long int count_prev = 0;
static String address = "227 - Nguyen Van Cu";
static float wr_contl = 0;
static float wr = 0;
SocketIoClient socketIO;
char host[] = "rewes1.glitch.me";
int port = 80;
uint64_t messageTimestamp;
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
int timeDelay = 1000;


StaticJsonDocument<500> StationDoc;
StaticJsonDocument<500> TempDoc;

BearSSL::X509List cert(AWS_CERT_CA);
BearSSL::X509List client_crt(AWS_CERT_CRT);
BearSSL::PrivateKey key(AWS_CERT_PRIVATE);
PubSubClient client(net);

time_t now;
time_t nowish = 1510592825;

void NTPConnect(void) {
  Serial.print("Setting time using SNTP");
  configTime(7*3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);

  while (now < nowish) {
    delay(500);
    Serial.print(".NTPConnect.");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}


void messageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void connectAWS() {
  delay(3000);
  WiFi.mode(WIFI_STA);

  NTPConnect();

  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.setServer(AWS_IOT_ENDPOINT, 8883);
  client.setCallback(messageReceived);


  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
           if (millis() - lastMillis > timeDelay*10)
    {
      lastMillis = millis();
          Serial.print(".connectAWS.");
          delay(1000);
          ESP.restart();
          

    }
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage() {
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  StaticJsonDocument<200> doc;
  timeClient.update();
  //unsigned long epochTime = timeClient.getEpochTime(); // Lấy epoch time (theo giây)
  //nsigned long long timestamp = (epochTime * 1000ULL) ;
  doc["LocalTime"] = asctime(&timeinfo);
  doc["timestamp"] = (timeClient.getEpochTime()*1000ULL);
  doc["humidity"] = humi;
  doc["temperature"] = tempC;
  doc["uSv"] = uSv;
  doc["cps"] = cps;
  doc["counts"] = count_present;
  doc["stationName"] = String(statusStation);

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);  // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


ICACHE_RAM_ATTR static void tube_impulse(void) {
  counts++;
}
void changedname(const char* message, size_t length) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);
  address = String(doc["namec"]);
  Serial.println("address");
}
void storagebysec(const char* message, size_t length) {
  wr = 0;
  Serial.print("storagebysec");
}
void storageby1min(const char* message, size_t length) {
  wr = 1;
  Serial.print("storageby1min");
}
void storageby5min(const char* message, size_t length) {
  wr = 5;
  Serial.print("storageby5min");
}
void connectEvent(const char* payload, size_t length) {
  StationDoc["id"] = "";
  Serial.println("Station ESP connectEvent");
  serializeJson(StationDoc, Serial);
  char msg[256];
  serializeJson(StationDoc, msg);
  socketIO.emit("create-station", msg);
}

void tempEvent() {
  TempDoc["address"] = address;
  TempDoc["uSv"] = uSv;
  TempDoc["cps"] = cps;
  TempDoc["humi"] = round(humi);
  TempDoc["tempC"] = round(tempC);
  TempDoc["counts"] = count_present;
  TempDoc["statusStation"] = String(statusStation);
  TempDoc["date"] = timeCount_present;
  TempDoc["realtime"] = timeCount_present;
  TempDoc["wr_contl"] = wr_contl;
  TempDoc["wr"] = wr;
  TempDoc["a"] = 5;
  char msg[256];
  serializeJson(TempDoc, msg);
  socketIO.emit("temp", msg);
}

void Init() {
  //Serial
  Serial.begin(115200);

  //EEPROM
  EEPROM.begin(512);

  //ArduinoOTA
  ArduinoOTA.begin();

  //time
  timeClient.begin();
  timeClient.setTimeOffset(7* 3600);  //Vietnam Timezone

  //DHT
  HT.begin();

  //LCD
  lcd.init();
  lcd.backlight();

  //socketIO
  socketIO.begin(host, port);

  //pinStick
  pinMode(PIN_TICK, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_TICK), tube_impulse, FALLING);
}

void eeprom() {
  if (char(EEPROM.read(addr)) == 't') {

    EEPROM.write(addr, 'f');
    EEPROM.commit();

  } else {
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(180);
    wifiManager.autoConnect("ESP8266");
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      EEPROM.write(addr, 't');
      EEPROM.commit();  //Store data to EEPROM
      ESP.restart();
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void arduinoOTA() {
  ArduinoOTA.setHostname(statusStation);
  ArduinoOTA.setPasswordHash("123456789");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
}

void socKetIO() {
  TempDoc["address"] = "227-Nguyen Van Cu";
  TempDoc["tempC"] = 0;
  TempDoc["humi"] = 0;
  TempDoc["uSv"] = 0;
  TempDoc["cps"] = 0;
  TempDoc["counts"] = 0;
  TempDoc["wr_contl"] = 0;
  TempDoc["wr"] = 0;


  StationDoc["name"] = String(statusStation);
  StationDoc["geolocation"]["latitude"] = 10;
  StationDoc["geolocation"]["longitude"] = 106;
  StationDoc["address"] = "227-NguyenVanCu";
  //StationDoc["id"] = "";
  


  socketIO.on("changename", changedname);
  socketIO.on("storagebysec", storagebysec);
  socketIO.on("storageby1min", storageby1min);
  socketIO.on("storageby5min", storageby5min);
}

void readData() {
  //read time
  timeClient.update();  // cập nhật server thời gian hiện tại
  unsigned long epochTime = timeClient.getEpochTime();

  //read DHT11
  humi = HT.readHumidity(); 
  tempC = HT.readTemperature(); 
  if (isnan(humi) || isnan(tempC))  // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //connect AWS
  now = time(nullptr);

if (!client.connected())
  {
    connectAWS();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > timeDelay)
    {
      lastMillis = millis();
      publishMessage();
    }
  }
  

  //read GM and connect socketIO
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    //connect socketIO
    socketIO.loop();
    socketIO.on("connect", connectEvent);

     //read GM 
    timeCount_present = epochTime;
    count_present = counts;
    if (timeCount_present != timeCount_prev) {
      //calculator cps, uSv
      cps = (count_present - count_prev) / (timeCount_present - timeCount_prev);
      uSv = cps * 60 / 151.0;
      count_prev = count_present;
      timeCount_prev = timeCount_present;
      if (wr == 0) {
        wr_contl = 0;
      } else if (wr == 1) {
        if (timeCount_present % 60 == 0) {
          wr_contl = 0;
          Serial.print("wr_contl = 0");
        } else {
          wr_contl = 1;
          Serial.print("wr_contl = 1");
        }
      } else if (wr == 5) {
        if (timeCount_present % 300 == 0) {
          wr_contl = 0;
        } else {
          wr_contl = 1;
        }
      }
      tempEvent();
    }
  } else {
    timeCount_present = 0;
    count_present = counts;
    if (millis() - timeTick > timeDelay) {
      //calculator cps, uSv
      cps = count_present - count_prev;
      uSv = cps * 60 / 151.0;
      count_prev = count_present;
      timeTick = millis();
    }
  }
}

void display() {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.setCursor(2, 0);
  lcd.print(tempC);
  lcd.setCursor(7, 0);
  lcd.print("C");

  lcd.setCursor(9, 0);
  lcd.print("C:");
  lcd.setCursor(11, 0);
  lcd.print(cps);

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.setCursor(2, 1);
  lcd.print(humi);
  lcd.setCursor(7, 1);
  lcd.print("%");

  lcd.setCursor(9, 1);
  lcd.print("U:");
  lcd.setCursor(11, 1);
  lcd.print(uSv);
}

void setup() {

  Init();
  eeprom();
  arduinoOTA();
  connectAWS();
  socKetIO();
}

void loop() {
  ArduinoOTA.handle();
  readData();
  display();
}