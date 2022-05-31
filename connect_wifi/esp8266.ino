#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

const char* ssid = "S20 fe Earn"; // S20 fe Earn
const char* password = "asdfghjkl"; //asdfghjkl
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "cc44a11b-b09e-4984-a576-a842c828ce46";
const char* mqtt_username = "2TvWBSXarywuzSf6yN42CKdQeSnUfEE9";
const char* mqtt_password = "4Hu#IBR_g)8L3)8!RdK5df1$5srh3hIS";

char msg[100];
String tmp = "";
int count = 0;
int current_people = 0;
int previous_people = 0;
String previous_state_s1 = "off";
String previous_state_s2 = "off";
String current_state_s1,current_state_s2;

// Set up a new SoftwareSerial object
SoftwareSerial mySerial;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("connected");
//      client.subscribe("@msg/led_control");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  //------uart-----
  // Define pin modes for TX and RX
  delay(1000);
  pinMode(D6, INPUT);
  pinMode(D8, OUTPUT);
  pinMode(D1, OUTPUT);
  // Set the baud rate for the SoftwareSerial object
  mySerial.begin(9600,SWSERIAL_8N1, D6, D8, false);
  //---------------
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, mqtt_port);
//  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
//   read from uart
  String data;
  bool send_data = false;
  String case_send = "";
  digitalWrite(D1,HIGH); // signal interupt
  if (mySerial.available() > 0) {
        char c = mySerial.read();
        if(c == '\n'){
          Serial.flush();
          Serial.println(tmp);
          count++;
          if(tmp == "S1:n"){
            current_state_s1 = "on";
          }else if(tmp == "S1:f"){
            current_state_s1 = "off";
          }else if(tmp == "S2:n"){
            current_state_s2 = "on";
          }else if(tmp == "S2:f"){
            current_state_s2 = "off";
          }
          if(count!=3) tmp = "";
        }
        else tmp += c;
        // sequence of data s1 -> s2 -> ultrasonic ( number of people)
        if(count == 3){
          delay(100);
          digitalWrite(D1,LOW);
          current_people = tmp.toInt();
          tmp = "";
          count = 0;
          if(current_people != previous_people || previous_state_s1 != current_state_s1 || previous_state_s2 != current_state_s2){
            send_data = true;
            if(previous_people > current_people) case_send += "2"; // person exits
            else if(previous_people < current_people) case_send += "1"; // person enter
            else if(previous_people == current_people) case_send += "0"; // no one eenter or exit
            if(current_state_s1 == "on") case_send += "1"; // light 1 on
            else if(current_state_s1 == "off") case_send += "0"; // light 1 off
            if(current_state_s2 == "on") case_send += "1"; // light 2 on
            else if(current_state_s2 == "off") case_send += "0"; // light 2 off
          }
        }
    }
  // update database
  if(send_data){
    previous_people = current_people;
    previous_state_s1 = current_state_s1;
    previous_state_s2 = current_state_s2;
    String data = "{\"data\": {\"people\":" + String(current_people) +",\"light1\":\""+current_state_s1 + "\",\"light2\":\""+current_state_s2 + "\"}}"; // send to @shadow/data/update 
    Serial.println(data);
    data.toCharArray(msg, (data.length() + 1));
    client.publish("@shadow/data/update", msg); 
    // send massage to html
    data = case_send;
    data.toCharArray(msg, (data.length() + 1));
    client.publish("@msg/feedback", msg);
  }
}

//void callback(char* topic, byte* payload, unsigned int length) {
//    Serial.print("Message arrive [");
//    Serial.print(topic);
//    Serial.print("]");
//    String message;
//    String data;
//    for (int i = 0; i < length; i++) {
//        message = message + (char)payload[i];
//    }
//    Serial.println(message);
//    // update database and send signal through uart 
//    if(message == "3"){
//        data = "{\"data\": {\"light1\": \"on\"}}";
//        Serial.write("00");
//    }
//    else if (message == "4"){
//        data = "{\"data\": {\"light1\": \"off\"}}";
//        Serial.write("01");
//    }
//    else if (message == "5"){
//        data = "{\"data\": {\"light2\": \"on\"}}";
//        Serial.write("10");
//    }
//    else if (message == "6"){
//        data = "{\"data\": {\"light2\": \"off\"}}";
//        Serial.write("11");
//    }
//    Serial.println(data);
//    data.toCharArray(msg, (data.length() + 1));
//    client.publish("@shadow/data/update", msg);
//}
