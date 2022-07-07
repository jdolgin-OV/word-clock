#ifndef _wifi_setup_H
#define _wifi_setup_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#define NUM_ATTEMPTS 1

const char* ssid = "BELL811";
const char* password = "jjma1234";

// Replace with your secondary network credentials
const char* ssid2 = "Josh iPhone";
const char* password2 = "1553church";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Web server html
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Omers WordClock</title>
  <style>
    html {font-family: Arial; display: inline-block}
    h2 {font-size: 3.5rem; text-align: center}
    p {font-size: 1.9rem;}
  table {width: 100%%}
  input {width: 80%%; border-radius: 10px}
  button {width: 100%%; height: 75px; font-size: 1.5rem; border-radius: 15px }
  button:hover{transition-duration: 0.5s; background-color: #555555; 
  color: white;}
  
    body {max-width: 500px; margin:0px auto; padding: 0px 7px; }
    .slider { -webkit-appearance: none; margin: 14px 0; width: 80%%; height: 12px; background: #cc6804; 
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 20px; height: 30px; background: #044485; cursor: grabbing; border-radius: 10px;}
    .slider::-moz-range-thumb { width: 25px; height: 25px; background: #044485; cursor: pointer; }
  .labelCol {width: 80px}
  .valCol {width: 40px; padding-left: 14px}
  </style>
</head>

<body>
  <h2>Omers WordClock</h2>
  <button type="button" id="nextBtn" onclick="sendData('n',1)" >Next pattern</button>

  </br></br>
  <table border="0">
  <tr>
</br>
  <label id="labelMessage" for="message">Message: </label>
  <input id="message" type="text" onchange="sendData('m',this.value)" value="%MESSAGE%">
  <br>
  <label id="labelSpeed" for="speedSlider">Speed: </label>
   <input type="range" id="speedSlider" onchange="sendData('s',this.value)" min="0" max="500" value="%SPEEDVALUE%" step="1" class="slider">
  </br>
  </form><br>
  
    <td class="labelCol"><label id="labelBrightness" for="brightnessSlider">Brightness</label></td>
    <td><input type="range" id="brightnessSlider" onchange="sendData('t',this.value)" min="0" max="255" value="200" step="1" class="slider"></td>
    <td class="valCol"><span id="brightnessValue">%BRIGHTNESSVALUE%</span></td>
  </tr><tr>
    <td class="sliderCol"><label id="labelRed" for="redSlider">Red</label></td>
    <td><input type="range" id="Red" onchange="sendData('r',this.value)" min="0" max="255" value="255" step="1" class="slider"></td>
    <td class="valCol"><span id="redValue">%REDVALUE%</span></td>
  </tr> <tr>
    <td class="labelRow"><label id="labelGreen" for="greenSlider">Green</label></td>
    <td><input type="range" id="greenSlider" onchange="sendData('g',this.value)" min="0" max="255" value="255" step="1" class="slider"></td>
    <td class="valCol"><span id="greenValue">%GREENVALUE%</span></td>
  </tr><tr>
    <td class="labelRow"><label id="labelBlue" for="blueSlider">Blue</label></td>
    <td><input type="range" id="blueSlider" onchange="sendData('b',this.value)" min="0" max="255" value="255" step="1" class="slider"></td>
    <td class="valCol"><span id="blueValue">%BLUEVALUE%</span></td>
  </tr> 
    
  </table>
<script>
 var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  
  function onOpen(event) {
    console.log('Connection opened');
  }
  
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  
  function onMessage(event) {
    var dataType = event.data.charAt(0);
    var dataValue = event.data.substring(1);
    switch (dataType){
    case 's':
      document.getElementById('speedValue').innerHTML = dataValue;
      document.getElementById('speedSlider').value = dataValue;
      break;
    case 't':
      document.getElementById('brightnessValue').innerHTML = dataValue;
      document.getElementById('brightnessSlider').value = dataValue;
      break;
    case 'r':
      document.getElementById('redValue').innerHTML = dataValue;
      document.getElementById('redSlider').value = dataValue;
      break;
    case 'g':
      document.getElementById('greenValue').innerHTML = dataValue;
      document.getElementById('greenSlider').value = dataValue;
      break;
    case 'b':
      document.getElementById('blueValue').innerHTML = dataValue;
      document.getElementById('blueSlider').value = dataValue;
      break;
    case 'm':
      document.getElementById('message').innerHTML = dataValue;
      break;
    }
  }
  
  function onLoad(event) {
    initWebSocket();
  }
  function sendData(type, val) {
    console.log(type+val);
    websocket.send(type+val);
  }
</script>
</body>
</html>
)rawliteral";

uint8_t brightness = 200;
uint8_t red = 255;
uint8_t green = 255;
uint8_t blue = 255;
//String text = "Welcome to OMERS Venture Capital";
String text = "!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVQXYZ[\]^_`abdefghijklmnopqrstuvwxyz{|}~";
uint16_t scroll_speed = 250;
uint8_t pointer = 0;    //8 bit pointer to character in message

uint8_t clock_mode = 0;
#define NUM_MODES 5
bool change_mode = false;


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.println((char*)data);
    String message = String((char*)data);
    char dataType = message.charAt(0);
    String dataValue = message.substring(1);

    switch (dataType) {
      case 's':
        scroll_speed = 510 - dataValue.toInt();
        ws.textAll(message);
        break;
      case 't':
        brightness = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'r':
        red = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'g':
        green = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'b':
        blue = dataValue.toInt();
        ws.textAll(message);
        break;
      case 'm':
        text = dataValue;
        pointer = 0;
        ws.textAll(message);
        break;
      case 'n':
        clock_mode = (clock_mode + 1) % NUM_MODES;
        bool change_mode = true;
        break;
    }
  }
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
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


String processor(const String& var){
  if(var == "SPEEDVALUE"){
    return String(scroll_speed);
  }  if(var == "BRIGHTNESSVALUE"){
    return String(brightness);
  }
  if(var == "REDVALUE"){
    return String(red);
  }
  if(var == "GREENVALUE"){
    return String(green);
  }
    if(var == "BLUEVALUE"){
    return String(blue);
  }
    if(var == "Message"){
    return text;
  }
  return "";
}

void setupWebServer(){

  uint8_t connectionAttempts = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay (1000);
    Serial.println("Connecting to primary WiFi ...");
    connectionAttempts++;
    if (connectionAttempts > NUM_ATTEMPTS) break;    
  }

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid2, password2);
    delay (1000);
    Serial.println("Connecting to secondary WiFi ...");
    connectionAttempts++;
    if (connectionAttempts > NUM_ATTEMPTS * 2) break;    
  }

  // Print ESP Local IP Address
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.begin();
}

#endif
