#include <SPI.h>
#include <WiFiNINA.h> // NANO 33 IoT에서 Wi-Fi 기능을 사용하기 위한 라이브러리 입니다.
#include <PubSubClient.h>

const char* ssid = "colson_KT_GIGA_2.4G"; // Wi-Fi의 SSID(이름)를 입력합니다.
const char* password = "701985ss**"; // Wi-Fi의 페스워드를 입력합니다.
const char* mqtt_server = "172.30.1.8"; // MQTT 서버를 입력합니다.
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password); // 앞서 설정한 ssid와 페스워드로 Wi-Fi에 연결합니다.
  while (WiFi.status() != WL_CONNECTED) { // 연결될 때 까지 0.5초 마다 Wi-Fi 연결상태를 확인합니다.
    delay(500);
  }
  randomSeed(micros()); // 렌덤 문자를 위한 렌덤 시드를 설정합니다.

  Serial.println("Connected to WiFi");
  printWifiStatus();
}

int current_sensor = 1;

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
    if (payload[0] == '1')
    {
      current_sensor = 0;
      Serial.println("Turn on relay");
    }
    if (payload[0] == '0')
    {
      current_sensor = 1;
      Serial.println("Turn off relay");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "switcher"; // 클라이언트 ID를 설정합니다.
    clientId += String(random(0xffff), HEX); // 같은 이름을 가진 클라이언트가 발생하는것을 방지하기 위해, 렌덤 문자를 클라이언트 ID에 붙입니다.
    if (client.connect("switcher", "", "")) { // 앞서 설정한 클라이언트 ID로 연결합니다.
      Serial.println("Connected");
      client.subscribe("cmd/switch1"); // inTopic 토픽을 듣습니다.
    } else {
      Serial.println("!!?!?!?");
      delay(5000);
    }
  }
}

long nowTick;
long pastTick;
void setup() {
  pinMode(2, OUTPUT);     // 2번 LED를 토글하기 때문에 OUTPUT으로 설정합니다.
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883); // MQTT 서버에 연결합니다.
  client.setCallback(callback);
  nowTick = millis();
  pastTick = nowTick;
  pinMode(2, OUTPUT);
  pinMode(A0, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  nowTick = millis();
  if (nowTick - pastTick > 500)
  {
    int value = analogRead(A0);
    int value2 = analogRead(A2);
  
    Serial.print("Analog value: ");
    Serial.println(value);
    Serial.print("Analog value2: ");
    Serial.println(value2);
    if (current_sensor == 0)
    {
      digitalWrite(2, LOW);
      client.publish("stat/switch1", "1");
    }
    else if (current_sensor == 1)
    {
      digitalWrite(2, HIGH);
      client.publish("stat/switch1", "0");
    }
    Serial.println("Send state done!");
    pastTick = nowTick;
  }
}

void printWifiStatus() {
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
