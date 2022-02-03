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

#define CURRENT 20

const char* ssid = "colson_KT_GIGA_2.4G";
const char* password = "701985ss**";
const char* mqtt_server = "172.30.1.22";

WiFiClient espClient;
PubSubClient client(espClient);

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
  if (strcmp(topic, "cmd/outlet-1/power") == 0)
  {
    Serial.println("Matched!");
    Serial.print("payload[0]: ");
    Serial.print(payload[0]);
    Serial.print(", ");
    Serial.println((char)payload[0]);
    if ((char)payload[0] == '0')
    {
      Serial.println("get 0");
      digitalWrite(2, HIGH);
    }
    else if((char)payload[0] == '1')
    {
      Serial.println("get 1");
      digitalWrite(2, LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "outlet-1"; // 클라이언트 ID를 설정합니다.
    clientId += String(random(0xffff), HEX); // 같은 이름을 가진 클라이언트가 발생하는것을 방지하기 위해, 렌덤 문자를 클라이언트 ID에 붙입니다.
    if (client.connect("outlet-1", "", "")) { // 앞서 설정한 클라이언트 ID로 연결합니다.
      Serial.println("Connected");
      client.subscribe("cmd/outlet-1/power"); // inTopic 토픽을 듣습니다.
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
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
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
    int value = analogRead(A0);
    int value2 = analogRead(A2);

    float volt20A = value * (3.3 / 1024);
    float current20A = abs(volt20A - 2.32) * (CURRENT / 2);
    float watt20A = volt20A * current20A;


    float volt5A = value2 * (3.3 / 1024);
    float current5A = abs(volt5A - 2.32) * (5 / 2);
    float watt5A = volt5A * current5A;
    
    Serial.print("value (MAX 20A): "); Serial.print(value); Serial.print(", ");
    Serial.print("volt: "); Serial.print(volt20A);
    Serial.print("watt: "); Serial.println(watt20A);
    Serial.print("value (MAX 5A): ");Serial.print(value2); Serial.print(", ");
    Serial.print("volt5A: "); Serial.print(volt5A);
    Serial.print("watt5A: "); Serial.println(watt5A);

    int state = digitalRead(2);

    client.publish("stat/outlet-2/volts", String("220").c_str());
    client.publish("stat/outlet-2/amperes", String(current20A).c_str());
    client.publish("stat/outlet-2/watts", String(watt20A).c_str());

    client.publish("stat/outlet-3/volts", String("220").c_str());
    client.publish("stat/outlet-3/amperes", String(current5A).c_str());
    client.publish("stat/outlet-3/watts", String(watt5A).c_str());
    
    if (state != 0)
      client.publish("stat/outlet-1/power", "0");
    else
      client.publish("stat/outlet-1/power", "1");

    pastTick = nowTick;
  }
}
