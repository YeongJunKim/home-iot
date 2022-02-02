/*
  HTU21D Humidity Sensor Example Code
  By: Nathan Seidle
  SparkFun Electronics
  Date: September 15th, 2013
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Uses the HTU21D library to display the current humidity and temperature

  Open serial monitor at 9600 baud to see readings. Errors 998 if not sensor is detected. Error 999 if CRC is bad.

  Hardware Connections (Breakoutboard to Arduino):
  -VCC = 3.3V
  -GND = GND
  -SDA = A4 (use inline 330 ohm resistor if your board is 5V)
  -SCL = A5 (use inline 330 ohm resistor if your board is 5V)

*/

#include <Wire.h>
#include <WiFiNINA.h> // NANO 33 IoT에서 Wi-Fi 기능을 사용하기 위한 라이브러리 입니다.
#include <PubSubClient.h>
#include "SparkFunHTU21D.h"

const char* ssid = "colson_KT_GIGA_2.4G";
const char* password = "701985ss**";
const char* mqtt_server = "172.30.1.41";

WiFiClient espClient;
PubSubClient client(espClient);

HTU21D myHumidity;

void setup_wifi()
{
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  String p = String((char *)payload);
  Serial.println(p);
  if (strcmp(topic, "cmd/switch1") == 0)
  {
    Serial.println("Matched!");
    Serial.print("payload[0]");
    Serial.print(payload[0]);
    Serial.println((char)payload[0]);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "htu21d"; // 클라이언트 ID를 설정합니다.
    clientId += String(random(0xffff), HEX); // 같은 이름을 가진 클라이언트가 발생하는것을 방지하기 위해, 렌덤 문자를 클라이언트 ID에 붙입니다.
    if (client.connect("htu21d", "", "")) { // 앞서 설정한 클라이언트 ID로 연결합니다.
      Serial.println("Connected");
      client.subscribe("cmd/test"); // inTopic 토픽을 듣습니다.
    } else {
      Serial.println("!!?!?!?");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("HTU21D Example!");
  setup_wifi();
  client.setServer(mqtt_server, 1883); // MQTT 서버에 연결합니다.
  client.setCallback(callback);

  myHumidity.begin();
}

long nowTick = 0;
long pastTick = 0;
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  nowTick = millis();
  if (nowTick - pastTick > 1000)
  {

    float humd = myHumidity.readHumidity();
    float temp = myHumidity.readTemperature();

    Serial.print("Time:");
    Serial.print(millis());
    Serial.print(" Temperature:");
    Serial.print(temp, 1);
    Serial.print("C");
    Serial.print(" Humidity:");
    Serial.print(humd, 1);
    Serial.print("%");
    Serial.println();

    client.publish("stat/htu21d/humidity", String(humd).c_str());
    client.publish("stat/htu21d/temperature", String(temp).c_str());
    client.publish("stat/htu21d/active", "1");
    client.publish("online/htu21d", "1");


    pastTick = nowTick;
  }
}
