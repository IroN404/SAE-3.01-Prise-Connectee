// Library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Define VAR
#define BUTTON_PIN 12  // ESP32 pin GIOP12, which connected to button
unsigned long lastTimeButtonStateChanged = 0;
unsigned long debounceDuration = 50; // millis
byte lastButtonState = LOW;
byte ledState = LOW;
int led_state = LOW;    // the current state of LED
int button_state;       // the current state of button
int last_button_state;  // the previous state of button

// defines leds
const int led1 = 0;
const int led2 = 2;

// WIFI
const char* ssid = "APsae";
const char* password = "password123";

// Broker
const char* mqtt_server = "192.168.1.39";
const char* mqtt_user = "toto"; // from psswd file in C:/mosquitto directory
const char* mqtt_password = "root"; //from psswd file in C:/mosquitto directory

// Connect to WIFI
WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
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


// get broker messages
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
// condition to put leds on or off
 if(topic=="saelight1"){
      Serial.print("Changing led1 to ");
      if(messageTemp == "1"){
        digitalWrite(led1,0);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(led1, 1);
        Serial.print("Off");
      }
  }
  if(topic=="saelight2"){
      Serial.print("Changing led2 to ");
      if(messageTemp == "1"){
        digitalWrite(led2, 0);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(led2, 1);
        Serial.print("Off");
      }
  }
  Serial.println();
}


// Connect to mqtt
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_password)) {
      Serial.println("connected");
      client.subscribe("saelight1");
      client.subscribe("saelight2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Setup
void setup() {
  pinMode(led1, OUTPUT); // ça déclare la led fin tu sais lire
  pinMode(led2,OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);    // initialize the pushbutton pin as a pullup button:
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


// Loop
void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (millis() - lastTimeButtonStateChanged > debounceDuration) {
    byte buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState) {
      
      lastTimeButtonStateChanged = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        ledState = (ledState == HIGH) ? LOW: HIGH;
    if (ledState == LOW){
        Serial.print("Publish message: ");
    Serial.println("on");
    client.publish("saelight1", "1");
        
    }else {Serial.print("Publish message: ");
    Serial.println("off");
    client.publish("saelight1", "0");}
    
      }
        digitalWrite(led1, ledState);
      }
    }
  }
  unsigned long now = millis();

// print message received on wich topic
void MQTTcallback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message MQTT [");
  Serial.print(topic);
  Serial.print("] ");

  payload[length] = '\0';
  String s = String((char*)payload);

 Serial.println("message reçu : "+s);
}
