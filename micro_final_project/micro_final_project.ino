#include <SPI.h>
#include <WiFi.h>
#define speakerPin D3
#define UPDATE_TIME 20
#define Human_Buzzer D4
#define servoPin_180 D5
#define LFservoPin D7
#define RFservoPin D6
#define LBservoPin D9
#define RBservoPin D8

#define NOTE_Med_DO  523
#define NOTE_Med_SO  784

//servo_360
char start = 0;
char level = 0;
int noRotate = 0;

int i;  
int shout = 1; //判斷是否叫
int Buzzer = 1; //要不要用蜂鳴器

//servo_180
int degree = 90;
int endcode;

char ssid[] = "Jason";      
char pass[] = "Jason921104";   

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void human_sensor(){
  int motionState = digitalRead(Human_Buzzer);
  if (motionState == HIGH) {
    Serial.println("Movement detected.");
    tone(speakerPin, NOTE_Med_DO);
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_Med_SO);
    delay(1000);
    noTone(speakerPin);
    shout = 1;
  } 
  else {
    Serial.println("Nothing.");
    shout = 0;
  }
}

void rotate(int LFpulse, int RFpulse, int LBpulse, int RBpulse){
  int i;
  noInterrupts(); // 停止中斷
  
  for(i=0; i<50; i++){
    digitalWrite(LFservoPin, HIGH);
    delayMicroseconds(LFpulse);
    digitalWrite(LFservoPin, LOW);
    digitalWrite(LBservoPin, HIGH);
    delayMicroseconds(LBpulse);
    digitalWrite(LBservoPin, LOW);
    
    digitalWrite(RFservoPin, HIGH);
    delayMicroseconds(RFpulse);
    digitalWrite(RFservoPin, LOW);
    digitalWrite(RBservoPin, HIGH);
    delayMicroseconds(RBpulse);
    digitalWrite(RBservoPin, LOW);
    delay(20);
  }
  interrupts(); // 恢復中斷
}

void rotation_rate(char level){
      switch(level){  
        case 0: break;
        case 1: rotate(1600,1400,1900,1300); break; //前進
        case 2: rotate(1400,1600,1100,1750); break; //後退
        case 3: rotate(1200,1200,1100,1200); break; //左轉
        case 4: rotate(1800,1600,2000,1600); break; //右轉
        default: break;
      }
}

int convertAngleToImp(int degree){
  int a = 2000/180;
  int b = 500;
  return int(a*degree+b);
}
void ServoControl(int degree) {
  digitalWrite(servoPin_180, HIGH);
  delayMicroseconds(convertAngleToImp(degree));
  digitalWrite(servoPin_180,LOW);
  delay(UPDATE_TIME);
}

void setup() {
  Serial.begin(9600);      
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   

    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();                           
  printWifiStatus();                        

  //人體 蜂鳴器
  pinMode(Human_Buzzer, INPUT);
  digitalWrite(Human_Buzzer, LOW);
  pinMode(speakerPin, OUTPUT);
  delay(1000);
  
  //360馬達
  pinMode(LFservoPin, OUTPUT);
  digitalWrite(LFservoPin, LOW);
  pinMode(RFservoPin, OUTPUT);
  digitalWrite(RFservoPin, LOW);
  pinMode(LBservoPin, OUTPUT);
  digitalWrite(LBservoPin, LOW);
  pinMode(RBservoPin, OUTPUT);
  digitalWrite(RBservoPin, LOW);
  delay(1000);

  //180馬達
  pinMode(servoPin_180, OUTPUT);
}
void loop() {
  WiFiClient client = server.available();   
  if (client) {                             
    Serial.println("WIFI Demo");           
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<font color=\"blue\" size=\"7\">======== WIFI DEMO ========</font><br>");
            client.print("<br><br>");
            client.print("<button style=\"font-size: 28px;\"><a href=\"-1\">ON/OFF</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"0\">Buzzer</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"1\">STOP</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"2\">Forward</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"3\">Backward</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"4\">Left</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"5\">Right</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"6\">45</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"7\">90</a></button><br>");
            client.print("<br><br>"); 
            client.print("<button style=\"font-size: 28px;\"><a href=\"8\">135</a></button><br>");
            client.print("<br><br>"); 

            client.println();
            break;
          } else {    
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
        if (currentLine.endsWith("GET /-1")) {
          noInterrupts(); 
          start = !start;
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /0")) {
          noInterrupts(); 
          if(start){
            Buzzer = !Buzzer;
            shout = 0;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /1")) {
          noInterrupts(); 
          if(start){
            level = 0;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /2")) {
          noInterrupts(); 
          if(start){
            level = 1;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /3")) {
          noInterrupts(); 
          if(start){
            level = 2;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /4")) {
          noInterrupts(); 
          if(start){
            level = 3;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /5")) {
          noInterrupts(); 
          if(start){
            level = 4;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /6")) {
          Serial.println("Received request to set angle to 45 degrees.");
          noInterrupts(); 
          if(start){
            degree = 45;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /7")) {
          Serial.println("Received request to set angle to 90 degrees.");
          noInterrupts(); 
          if(start){
            degree = 90;
          }
          interrupts(); 
        }
        else if (currentLine.endsWith("GET /8")) {
          Serial.println("Received request to set angle to 135 degrees.");
          noInterrupts(); 
          if(start){
            degree = 135;
          }
          interrupts(); 
        }
      }
    }
    client.stop();
    delay(500);
    Serial.println("cdis");
  }
  if(start){
    if(Buzzer==1){
      human_sensor();
    }
    if(shout==0){
      rotation_rate(level);
    }
    ServoControl(degree);
  }
}
