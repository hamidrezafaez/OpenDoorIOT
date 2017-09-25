
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Servo.h> 
#define SERIAL_LOG

Servo myservo1;
Servo myservo2;
int r = 2;//D4
int g = 14;//D5
int b = 12;//D6
int remote = 16;//D0
int serv1 = 5;//D1
int serv2 = 4;//D2
int pos = 0;
const char* host = "5.160.71.117";
const int port = 2000;
int timer = 0;
bool ConnectWifi = false;
bool connectServer = false;
String SN = "11112";
String ServerMessage = "";
/* Set these to your desired credentials. */
const char *ssid = "OpenDoor:)";
const char *password = "";
String sid;
String pass;

IPAddress Ip(192, 168, 1, 1);
IPAddress NMask(255, 255, 255, 0);
ESP8266WebServer server(80);
WiFiClient SocketServer;
/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  server.send(200, "text/html", "<h1>Welcome To My Page</h1>");
}

void handlessidpass(){
  //http://192.168.1.1/sid?s=WING&p=CreateFuture1495
  sid = server.arg("s");
  pass = server.arg("p");
  const char *ss = sid.c_str();
  const char *pp = pass.c_str();
  Serial.println(ss);
  Serial.println(pp);
  server.send(200, "text/html", "<h1>Please Wait</h1>");
  WiFi.begin(ss, pp);
  #ifdef SERIAL_LOG
    Serial.print("\nConnecting to "); Serial.println(ss);
  #endif
  uint8_t i = 0;
  lightControl('b');
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    #ifdef SERIAL_LOG
    Serial.print("Could not connect to"); Serial.println(ss);
    lightControl('r');
    #endif
  }
  else{
    Serial.println("Connected");
  }
}

void setup() {
  myservo1.attach(serv1);
  myservo2.attach(serv2);
  myservo1.write(120);
  myservo2.write(10);
  pinMode(r,OUTPUT);
  pinMode(g,OUTPUT);
  pinMode(b,OUTPUT);
  //pinMode(7,OUTPUT);
  pinMode(remote,OUTPUT);
  Serial.setTimeout(1);
  lightControl('r');
  delay(1000);
  WiFi.softAPConfig(Ip, Ip, NMask);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  String clientMac = "";
  unsigned char mac[6];
  WiFi.macAddress(mac);
  clientMac += macToStr(mac);
  Serial.println("Mac Address : "+clientMac);
  //--------------------
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/sid",handlessidpass);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if(!SocketServer.connect(host, port)){
    #ifdef SERIAL_LOG
      Serial.println("connection failed");
      lightControl('r');
      delay(5000);
    #endif
    return; 
    }
 
    #ifdef SERIAL_LOG
    if(SocketServer.connected()){
        Serial.println("Server Connected");
        lightControl('g');
        String SNOfArduino = "{\"SN\":\""+SN+"\"}";
        SocketServer.print(SNOfArduino);
    }
    #endif  
    while(SocketServer.connected()){
      //server.end();
      WiFi.softAPdisconnect();
     if (SocketServer.available()) {
       ServerMessage = "";
       while(SocketServer.available()){
        char c = SocketServer.read();
        ServerMessage += c;
        Serial.print(c);  
       }  
       if(ServerMessage.indexOf("moto")!=-1){
        for (pos = 70; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
          myservo1.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        delay(1000);
        myservo2.write(60);
        delay(400); 
        for (pos = 60; pos >= 10; pos -= 1) { // goes from 0 degrees to 180 degrees
          myservo2.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        for (pos = 70; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
          myservo1.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        String OpenDoor = "{\"status\":\"OK\"}";//{"status":"OK"}
        SocketServer.print(OpenDoor);
       }
       else if(ServerMessage.indexOf("Remo") != -1||ServerMessage.indexOf("moted") != -1){
        digitalWrite(remote,1);
        delay(500);
        digitalWrite(remote,0);
      }
    }
    //Send messages to server
    //Envia as menssagens para o client
    bool message_available = (Serial.available() > 0);
    String message;
    if (message_available) {
        delay(200);
        message = Serial.readStringUntil('\n');
        SocketServer.print(message);
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


String macToStr(const uint8_t* mac)
{
String result;
for (int i = 0; i < 6; ++i) {
result += String(mac[i], 16);
if (i < 5)
result += ':';
}
return result;
}
