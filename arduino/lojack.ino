#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <GPRS_Shield_Arduino.h>
#include <Suli.h>
#include <Wire.h>
#include <String.h>

// GPS initialization:
HardwareSerial gpsSerial = Serial2;
Adafruit_GPS GPS(&gpsSerial);
void setupInterrupt();
uint32_t timer = millis();

// GPRS initialization:
#define PIN_TX    10
#define PIN_RX    11
#define BAUDRATE  19200
GPRS gprs(PIN_TX, PIN_RX, BAUDRATE,"cmnet");
SoftwareSerial gprsSerial(10,11);
char http_cmd[] = "GET /reportLocation.php?latitude=00.0000&longitude=-000.0000&deviceId=1 HTTP/1.0\r\n\r\n";

char remote_server[] = "FQDN.example.com";

SIGNAL(TIMER0_COMPA_vect) {char c = GPS.read();}

void setup()
{
  Serial.begin(115200);
  setupGPS();
  setupGprs();  
}

void setupGPS()
{
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  setupInterrupt();
  delay(1000);
  gpsSerial.println(PMTK_Q_RELEASE);
}

void setupInterrupt() {
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void setupGprs()
{
  pinMode(9, OUTPUT);
  digitalWrite(9,LOW);
  delay(100);
  digitalWrite(9,HIGH);
  delay(500);
  digitalWrite(9,LOW);
  delay(100);
  gprs.init();
}

void joinNetwork()
{
  while(false == gprs.join()) {
      Serial.println("gprs join network error");
      setupGprs();
      delay(2000);
  }
  Serial.println("joined network!");

  // successful DHCP
  Serial.print("IP Address is ");
  Serial.println(gprs.getIPAddress());

  if(false == gprs.connect(TCP,remote_server, 80)) {
      Serial.println("connect error");
  }else{
      Serial.println("connect success");
  }

}

void loop()
{
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
  if (timer > millis())  timer = millis();
  if (millis() - timer > 2000) { 
    timer = millis();
    sendLocation();
    Serial.print(GPS.latitudeDegrees, 4);Serial.print(", ");Serial.println(GPS.longitudeDegrees, 4);
  }
}

void updateRequestString()
{
  char charLatitude[10];
  char charLongitude[10];
  dtostrf(GPS.latitudeDegrees, 7, 4, charLatitude);
  dtostrf(GPS.longitudeDegrees, 9, 4, charLongitude);

  for (int i=0;i<7;i++) {http_cmd[33+i] = charLatitude[i];}
  for (int i=0;i<9;i++) {http_cmd[51+i] = charLongitude[i];}
}

void sendLocation()
{
  joinNetwork();
  updateRequestString();

  Serial.print("Sending: ");
  Serial.println(http_cmd);
  char buffer[512];
  gprs.send(http_cmd, sizeof(http_cmd)-1);
  while (true) {
      int ret = gprs.recv(buffer, sizeof(buffer)-1);
      if (ret <= 0){
          Serial.println("fetch over...");
          break; 
      }
      buffer[ret] = '\0';
      Serial.print("Recv: ");
      Serial.print(ret);
      Serial.print(" bytes: ");
      Serial.println(buffer);
  }
  gprs.close();
  gprs.disconnect();
}

