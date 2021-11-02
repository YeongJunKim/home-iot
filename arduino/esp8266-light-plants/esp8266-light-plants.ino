// esp8266_mqtt_client.ino
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int LED_PIN = 16;
int led_state = 1;
long last_send_time = 0;
char publish_msg[16];

const char* WIFI_SSID = "colson_KT_GIGA_2.4G";
const char* WIFI_PW = "701985ss**";
const char* MQTT_BROKER_ADDR = "172.30.1.8";
const int   MQTT_BROKER_PORT = 1883;
const char* MQTT_ID = ""; // optional
const char* MQTT_PW = "";  // optional

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, led_state);

  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PW);
  Serial.print("WiFi Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());

  // setup MQTT Client
  mqtt_client.setServer(MQTT_BROKER_ADDR, MQTT_BROKER_PORT);
  mqtt_client.setCallback(mqtt_callback);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (int(payload[length - 1] == '1')) {
    led_state = 1;
    digitalWrite(LED_PIN, 0);
  } else {
    led_state = 0;
    digitalWrite(LED_PIN, 1);
  }
  last_send_time = millis();
  sprintf(publish_msg, "%d", led_state);
  mqtt_client.publish("stat/plants/light", publish_msg);
}

void establish_mqtt_connection() {
  if (mqtt_client.connected())
    return;

  while (!mqtt_client.connected()) {
    Serial.println("Try to connect MQTT Broker");
    if (mqtt_client.connect("light_plants", MQTT_ID, MQTT_PW)) {
      Serial.println("Connected");
      mqtt_client.subscribe("cmd/plants/light");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      delay(5000);
    }
  }
}

void loop() {
  establish_mqtt_connection();
  mqtt_client.loop();

  long current = millis();
  if (current - last_send_time > 1000) {
    Serial.println("Send current state");
    last_send_time = current;
    sprintf(publish_msg, "%d", led_state);
    mqtt_client.publish("stat/plants/light", publish_msg);
  }
}
