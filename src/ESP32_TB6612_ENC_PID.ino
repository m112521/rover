// #include <Arduino.h>
#include <TB6612_ESP32.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#define ENCA1 18 // YELLOW
#define ENCA2 19 // WHITE

#define ENCB1 22 // YELLOW
#define ENCB2 23 // WHITE

#define AIN1 13 // ESP32 Pin D13 to TB6612FNG Pin AIN1
#define BIN1 12 // ESP32 Pin D12 to TB6612FNG Pin BIN1
#define AIN2 14 // ESP32 Pin D14 to TB6612FNG Pin AIN2
#define BIN2 27 // ESP32 Pin D27 to TB6612FNG Pin BIN2
#define PWMA 26 // ESP32 Pin D26 to TB6612FNG Pin PWMA
#define PWMB 25 // ESP32 Pin D25 to TB6612FNG Pin PWMB
#define STBY 33 // ESP32 Pin D33 to TB6612FNG Pin STBY

const char* ssid = "GalaxyAzamat";
const char* password = "vhrg8328";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

JSONVar readings;

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY, 5000, 8, 1);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY, 5000, 8, 2);

volatile int posiA1 = 0;
volatile int posiB1 = 0;

long prevT = 0;
float eprev = 0;
float eprev2 = 0;
float eintegral = 0;

int target = 0;

double kp = 1;
double kd = 0.025;
double ki = 0.0;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<p>Target=<span id="s1"></span></p>
<input id="target" type="number"/>
<hr/>
<p>Current=<span id="s5"></span></p>
<hr/>
<p>P=<span id="s2"></span></p>
<input id="p-textbox" type="number" value="1"/>
<hr/>
<p>D=<span id="s3"></span></p>
<input id="d-textbox" type="number" value="0.025"/>
<hr/>
<p>I=<span id="s4"></span></p>
<input id="i-textbox" type="number" value="0.0"/>

<!-- <input id="slider" type="range" min="0" max="255"/> -->
<button id="yo">SET</button>
<script>
let gateway = `ws://${window.location.hostname}/ws`;
let target = document.querySelector("#target");
let kp = document.querySelector("#p-textbox");
let kd = document.querySelector("#d-textbox");
let ki = document.querySelector("#i-textbox");

let websocket;
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
    initButtons();
}

function initButtons() {
  document.getElementById('yo').addEventListener('click', forwardMove);
}

function forwardMove(){
  //websocket.send('f');
  console.log(parseFloat(ki.value));
  websocket.send(JSON.stringify({target: parseInt(target.value),kp:parseInt(kp.value),kd:parseFloat(kd.value),ki:parseFloat(ki.value)}));
}

function getReadings(){
    websocket.send("getReadings");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
    let myObj = JSON.parse(event.data);
    let keys = Object.keys(myObj);

    for (let i = 0; i < keys.length; i++){
        let key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
    }
    websocket.send("getReadings");
}</script>
)rawliteral";

void IRAM_ATTR ISRA1(){
  int b = digitalRead(ENCA2);
  if(b > 0){
    posiA1++;
  }
  else{
    posiA1--;
  }
}

void IRAM_ATTR ISRB1(){
  int b = digitalRead(ENCB2);
  if(b > 0){
    posiB1++;
  }
  else{
    posiB1--;
  }
}

String getSensorReadings(){
  // int val = analogRead(ptrPin);
  // Serial.println(val);
  readings["s1"] = String(target);
  readings["s2"] =  String(kp);
  readings["s3"] = String(kd);
  readings["s4"] = String(ki);
  readings["s5"] = String(posiA1);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    
    JSONVar myObject = JSON.parse((const char*)data);
    if (myObject.hasOwnProperty("target")) {
      target = (int)myObject["target"];
      kp = (int)myObject["kp"];
      ki = (double)myObject["ki"];
      kd = (double)myObject["kd"];
      // Serial.print("myObject[\"target\"] = ");
      // ledcWrite(ledChannel, brightness);
    }

    String sensorReadings = getSensorReadings();
    //Serial.println(sensorReadings);
    notifyClients(sensorReadings);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  Serial.begin(115200);
  
  pinMode(ENCA1,INPUT);
  pinMode(ENCA2,INPUT);
  pinMode(ENCB1,INPUT);
  pinMode(ENCB2,INPUT);

  attachInterrupt(ENCA1, ISRA1, RISING);
  attachInterrupt(ENCB1, ISRB1, RISING);
  //testRun();

  initWiFi();
  initWebSocket();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
}

void loop()
{
  //int target = 325*sin(prevT/1e6);
  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  int posA1 = 0;
  //ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    posA1 = posiA1;
  //}

  // error
  int e = posA1 - target;
  // derivative
  float dedt = (e-eprev)/(deltaT);
  // integral
  eintegral = eintegral + e*deltaT;
  // control signal
  float u = kp*e + kd*dedt + ki*eintegral;

  // motor power
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }

  // motor direction
  int dir = 1;
  if(u<0){
    dir = -1;
  }

  //setMotor(dir,pwr,PWM,IN1,IN2);
  forward(motor1, motor2, dir*pwr);

  // store previous error
  eprev = e;

  // Serial.print(target);
  // Serial.print(" ");
  // Serial.print(posA1);
  // Serial.println();
    // forward(motor1, motor2, 255);
    // delay(2000); // Short delay to catch your robot
    // motor1.drive(255, 2000);       // Turn Motor 1 for 2 seconds at full speed
    // //motor1.drive(-255,2000);
    // motor1.brake();
    // forward(motor1, motor2, 255); //back(motor1, motor2, -255); // Reverse Motor 1 and Motor 2 for 1 seconds at full speed
    // left(motor1, motor2, 255); // brake(motor1, motor2);
}
