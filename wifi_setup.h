#ifndef _wifi_setup_H
#define _wifi_setup_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#define NUM_ATTEMPTS 5

const char* ssid = "";
const char* password = "";

// Replace with your secondary network credentials
const char* ssid2 = "";
const char* password2 = "";

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
  <h2>OMERS WordClock</h2>
  <button type="button" id="powerBtn" onclick="sendData('p',1)" >Power</button>
  <table>
      <tr>
        <td>
            <button type="button" id="nextBtn" onclick="sendData('a',1)" >Auto Change</button>
        </td>
        <td>
            <button type="button" id="nextBtn" onclick="sendData('n',1)" >   Next Pattern   </button>
        </td>
    </tr>
</table>
  <table>
     <tr>
        <td>
            <form action="/action_page.php">
             <label for="favcolor">Select color:</label>
        </td>
         <td>  
             <input type="color" id="favcolor" name="favcolor" value="FAVCOLORVALUE" onChange="sendData('c', this.value)">
             </form>
        </td>
    </tr>
    <tr>
        <td>
            <label id="labelMessage" for="message">Message: </label>
        </td>
        <td>
            <input id="message" type="text" onchange="sendData('m',this.value)" value="%MESSAGE%">
       </td>
   </tr>
   <tr>
       <td>
           <label id="labelSpeed" for="speedSlider">Speed: </label>
       </td>
       <td> 
           <input type="range" id="speedSlider" onchange="sendData('s',this.value)" min="0" max="500" value="%SPEEDVALUE%" step="1" class="slider"> 
       </td>
   </tr>
   <tr>
       <td> 
          <label id="labelBrightness" for="brightnessSlider">Brightness: </label>
       </td>
       <td>
       <input type="range" id="brightnessSlider" onchange="sendData('t',this.value)" min="0" max="255" value="200" step="1" class="slider">
       </td>
  </form>
  </tr>
  </table>
<br>
  
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
    //get the dataType
    switch (dataType){
    case 's':
      document.getElementById('speedValue').innerHTML = dataValue;
      document.getElementById('speedSlider').value = dataValue;
      break;
    case 't':
      document.getElementById('brightnessValue').innerHTML = dataValue;
      document.getElementById('brightnessSlider').value = dataValue;
      break;
    case 'c':
      document.getElementById('color').innerHTML = dataValue;
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

//the data being modified by the site
uint8_t brightness = 200;
unsigned long red = 247;
unsigned long green = 200;
unsigned long blue = 69;
String text = "Welcome to OV";
//String text = "!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVQXYZ[\]^_`abdefghijklmnopqrstuvwxyz{|}~";
uint16_t scroll_speed = 400;
uint8_t pointer = 0;    //8 bit pointer to character in message

//number of modes
uint8_t clock_mode = 0;
#define NUM_MODES 6
bool change_mode = false;

//whether to auto change modes
bool auto_change = false;

bool power = true;

String col = "";

//Interrupt routine to update the clock mode
void update_mode()
{
  //DIY button debounce, only interrupt every 1000 ms
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 1000)
  {
    //Iterate mode
    clock_mode = (clock_mode + 1) % NUM_MODES;
    change_mode = true;
    FastLED.clear();
    last_interrupt_time = interrupt_time;
  }
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.println((char*)data);
    String message = String((char*)data);
    char dataType = message.charAt(0);
    String dataValue = message.substring(1);
    //based on the character adjust the right variables
    switch (dataType) {
      case 's':
      {
        scroll_speed = 510 - dataValue.toInt();
        ws.textAll(message);
        break;
      }
      case 't':
      {
        brightness = dataValue.toInt();
        ws.textAll(message);
        break;
      }
      case 'c':
      {
        //break hex color value into each band
        col = dataValue + " ";
        String subRed = col.substring(1, 3);
        String subGreen = col.substring(3, 5);
        String subBlue = col.substring(5, 7);
        red = strtoul(subRed.c_str(), NULL, 16);
        green = strtoul(subGreen.c_str(), NULL, 16);
        blue = strtoul(subBlue.c_str(), NULL, 16);
        ws.textAll(message);
        ws.textAll(message);
        break;
      }
      case 'm':
      {
        //if user enters text readjust pointer
        text = dataValue;
        pointer = 0;
        ws.textAll(message);
        break;
      }
      case 'n':
      {
        update_mode();  
        break;
      }
      case 'a':
      {
        auto_change = !auto_change;
        break;
      }
      case 'p':
      {
        power = !power;
        break;
      }
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
  }  
  if(var == "BRIGHTNESSVALUE"){
    return String(brightness);
  }
  if(var == "FAVCOLORVALUE"){
    return col;
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

  WiFi.begin(ssid2, password2);
  while (WiFi.status() != WL_CONNECTED) {
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
