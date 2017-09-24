#include <ESP8266WiFi.h>
#include <Servo.h> 
#define SERIAL_LOG

WiFiClient server;
Servo myservo1;
Servo myservo2;
int r = 2;
int g = 14;
int b = 12;
int remote = 16;
int serv1 = 5;
int serv2 = 4;
int pos = 0;
const char* ssid     = "hrf 2";
const char* password = "12345678";
const char* host = "5.160.71.117";
const int port = 2000;
int timer = 0;
bool ConnectWifi = false;
bool connectServer = false;
String SN = "11112";
String ServerMessage = "";
 
void setup() {
  myservo1.attach(serv1);
  myservo2.attach(serv2);
  pinMode(r,OUTPUT);
  pinMode(g,OUTPUT);
  pinMode(b,OUTPUT);
  //pinMode(7,OUTPUT);
  pinMode(remote,OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(1);
  lightControl('r');
  WiFi.begin(ssid, password);

  #ifdef SERIAL_LOG
  Serial.print("\nConnecting to "); Serial.println(ssid);
  #endif
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    #ifdef SERIAL_LOG
    Serial.print("Could not connect to"); Serial.println(ssid);
    #endif
    while(1) delay(500);
  }
}
 
void loop() {
  lightControl('g');
  if(!server.connect(host, port)){
    #ifdef SERIAL_LOG
      Serial.println("connection failed");
      delay(5000);
    #endif
    return; 
    }
 
    #ifdef SERIAL_LOG
    if(server.connected()){
        Serial.println("Server Connected");
        String SNOfArduino = "{\"SN\":\""+SN+"\"}";
        server.print(SNOfArduino);
    }
    #endif  
    while(server.connected()){
 
     if (server.available()) {
       ServerMessage = "";
       while(server.available()){
        char c = server.read();
        ServerMessage += c;
        Serial.print(c);  
       }  
       if(ServerMessage.indexOf("moto")!=-1){
        for (pos = 0; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
          myservo1.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        for (pos = 120; pos >= 0; pos -= 1) { // goes from 0 degrees to 180 degrees
          myservo2.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        String OpenDoor = "{\"status\":\"OK\"}";//{"status":"OK"}
        server.print(OpenDoor);
       }
    }
    //Send messages to server
    //Envia as menssagens para o client
    bool message_available = (Serial.available() > 0);
    String message;
    if (message_available) {
        delay(200);
        message = Serial.readStringUntil('\n');
        server.print(message);
    }
  }
    //End of while means client is disconnected
    //Fim do while quer dizer que o server caiu
  #ifdef SERIAL_LOG
    Serial.println("Server Down");
  #endif
}


void lightControl(char light){
  if(light == 'g'){
    digitalWrite(r,0);
    digitalWrite(g,1);
    digitalWrite(b,0);
  }
  else if(light == 'r'){
    digitalWrite(r,1);
    digitalWrite(g,0);
    digitalWrite(b,0);
  }
  else{
    digitalWrite(b,1);
    digitalWrite(r,0);
    digitalWrite(g,0);
  }
};
