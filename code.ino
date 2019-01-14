#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
#define status_led BUILTIN_LED 
#define load1 2
#define input1 4


const char* ssid = "home_wg";
const char* password = "omisoksbwn";
const char* mqtt_server = "192.168.0.210";

WiFiClient espClient;
PubSubClient client(espClient);

IPAddress ip(192, 168, 0, 217); 
IPAddress gateway_dns(192, 168, 0, 1);

long lastMsg = 0;
char msg[50];
int value = 0;

int last_pub_time=0;



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.config(ip, gateway_dns, gateway_dns); 
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1')
    digitalWrite(load1, HIGH);
  else
    digitalWrite(load1, LOW);
      

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "lens_5kchta2dWhdRS2AQz0TeTuhplPE";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"openhabian", "openhabian")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("/home/esp8266/switch/command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(status_led, OUTPUT); 
  pinMode(load1, OUTPUT); 
  pinMode(input1, INPUT_PULLUP);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!digitalRead(input1) && (millis()- last_pub_time > 3000)){
          last_pub_time=millis();
          client.publish("/home/esp8266/switch/state","1");
  }
  client.loop();
}
