#include <Servo.h>

#define connectserver "AT+CIPSTART=\"TCP\",\"192.168.125.153\",2000"
#define Reset "AT+RST\r\n"
#define changemode "AT+CWMODE=2\r\n"
#define setip "AT+CIPAP=\"192.168.1.1\"\r\n"
#define multiconnect "AT+CIPMUX=1\r\n"
#define creatserver "AT+CIPSERVER=1,80\r\n"
#define connecting "AT+CWJAP="
#define getipaddress "AT+CIFSR"
#define senddata "AT+CIPSEND="
#define connectserver "AT+CIPSTART=\"TCP\",\"192.168.43.244\",2000"
#define senddata "AT+CIPSEND="



Servo myservo1;
Servo myservo2;
int pos = 0;
String command = "";
String ssid="";
String pass="";
int timer = 1400;

void setup() {
  myservo1.attach(8);
  myservo2.attach(9);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  // put you\r setup code here, to run once:
  Serial.begin(115200);
  delay(200);
  Serial.print(Reset);
  delay(4000);
  Serial.print(changemode);
  delay(2000);
  Serial.print(setip);
  delay(2000);
  Serial.print(multiconnect);
  delay(3000);
  Serial.print(creatserver);
  //Serial.print(getipaddress);
  delay(2000);
  digitalWrite(4,0);
  digitalWrite(5,1);
  digitalWrite(6,1);
  digitalWrite(7,1);
}

void loop() {
    //--------------------------------------------------------------
  if (Serial.available() > 0) {
    String com = "";
    while (Serial.available())
    {
      char c = Serial.read();
      com += (char)c;
    }
    delay(300);
    if(com.indexOf("+IPD,")){
      delay(100);
    //sample request 192.168.1.1/ssidIrancell-Z6000passfaez22338510
      if(com.indexOf("ssid") != -1){
        //digitalWrite(7,1);
        Serial.print("AT+CIPSERVER=0\r\n");
        delay(100);
        Serial.print("AT+CIPMUX=0\r\n");
        delay(200);
        Serial.print("AT+CWMODE=1\r\n");
        delay(200);
        int sid =  com.lastIndexOf("ssid");
        int pas =  com.lastIndexOf("pass");
        int http = com.indexOf("HTTP");
        int len =  com.length();
  
        ssid=com.substring(sid+4, pas);
        pass= com.substring(pas+4, http-1);
        //ChangeSSID = true;
        String ConnectCommand="";
        ConnectCommand += connecting;
        ConnectCommand += "\"" + ssid + "\",\"" + pass + "\"\r\n";
    
        ConnectCommand.replace("%20", " ");

        Serial.print(ConnectCommand);
        digitalWrite(4,1);
        digitalWrite(5,1);
        digitalWrite(6,0);
        delay(100);
        //ChangeSSID = false;
        
      }
      else if(com.indexOf("moto") != -1){
//        myservo1.write(0);
//        delay(200);
//        myservo2.write(0);
        for (pos = 0; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
          myservo1.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        for (pos = 0; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
          myservo2.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        String Message = "{\"status\":\"OK\"}";
        String lenght = senddata;
        lenght += Message.length();
        Serial.println(lenght);
        delay(100);
        Serial.println(Message);
      }
      else if(com.indexOf("FAIL") != -1){
        digitalWrite(4,0);
        digitalWrite(5,1);
        digitalWrite(6,1);
        Serial.print("AT+CWMODE=2\r\n");
        delay(200);
        Serial.print(multiconnect);
        delay(1500);
        Serial.print(creatserver);
        delay(200);
      }
      else if(com.indexOf("GOT") != -1){
        digitalWrite(4,1);
        digitalWrite(5,0);
        digitalWrite(6,1);
      }
    }
  }
  //--------------------------------------------------------------
  if (timer == 2500) {
      Serial.println(connectserver);
      digitalWrite(7,0);
  }
    //-------------------------------------------------------------
    delay(10);
    if(timer++ == 3000){
      timer = 0;
      digitalWrite(7,1);
    }
    //digitalWrite(10,1);
}


