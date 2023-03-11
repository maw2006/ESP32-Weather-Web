// Importing necessary libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Setting network credentials
const char* ssid = "netis";
const char* password = "wahas1122";

const char* input_parameter1 = "output";
const char* input_parameter2 = "state";

// Creating a AsyncWebServer object 
AsyncWebServer server(80);

#define BUTTON_PIN 0  // ESP32 pin GIOP16, which connected to button
#define LED_PIN    2  // ESP32 pin GIOP18, which connected to led

// The below are variables, which can be changed
int led_state = LOW;    // the current state of LED
int button_state;       // the current state of button
int last_button_state;  // the previous state of button

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP2 WEB SERVER</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP2 WEB SERVER</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Output - PIN 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";

//    buttons += "<h4>Output - GPIO 25</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"25\" " + outputState(25) + "><span class=\"slider\"></span></label>";
//
//    buttons += "<h4>Output - GPIO 27</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"27\" " + outputState(27) + "><span class=\"slider\"></span></label>";

//   buttons += "<h4>Output - GPIO 13</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"13\" " + outputState(13) + "><span class=\"slider\"></span></label>";

    return buttons;
  }
  return String();
}

String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}

void setup(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ESP32 Web Server");
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(ssid);
  lcd.print(" Connecting");

  // Serial port for debugging purposes
//  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // set ESP32 pin to input pull-up mode
  pinMode(LED_PIN, OUTPUT);          // set ESP32 pin to output mode
  digitalWrite(LED_PIN, LOW);
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);
  pinMode(27, OUTPUT);
  digitalWrite(27, LOW);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  button_state = digitalRead(BUTTON_PIN);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  int i = 1;
  lcd.setCursor(0,1);
  
  while (WiFi.status() != WL_CONNECTED) {
        lcd.print(".");
        i+=1;
        if( i == 18 ) {
          i=1;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(ssid);
          lcd.print(" Connecting");
          lcd.setCursor(0,1);
          }
    delay(500);
//    Serial.println("Connecting to WiFi");
  }

  // Print ESP Local IP Address
//  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IP:");
  lcd.setCursor(3,0);
  lcd.print(WiFi.localIP());
//  lcd.setCursor(0,1);
//  lcd.print("All IS OFF");
  delay(1000);
  lcd.noBacklight(); // turn off backlight
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(input_parameter1) && request->hasParam(input_parameter2)) {
      inputMessage1 = request->getParam(input_parameter1)->value();
      inputMessage2 = request->getParam(input_parameter2)->value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
//    Serial.print("GPIO: ");
//    Serial.print(inputMessage1);
//    Serial.print(" - Set to: ");
//    Serial.println(inputMessage2);
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("IP:");
//  lcd.setCursor(3,0);
//  lcd.print(WiFi.localIP());
//          lcd.setCursor(0,1);
//          lcd.print(inputMessage1);
//          lcd.print(" -is ");
//          lcd.print(inputMessage2);
//          lcd.print("  ");
    if (inputMessage2 == "1")
      lcd.backlight(); // turn on backlight.
    else
      lcd.noBacklight(); // turn off backlight
        
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {
//   if (input_parameter1 == "1")
//    led_state = HIGH;
//   else
//    led_state = LOW;
//    
  last_button_state = button_state;      // save the last state 
  button_state = digitalRead(BUTTON_PIN); // read new state

  if (last_button_state == HIGH && button_state == LOW) {

    // toggle state of LED
    led_state = !led_state;

    // control LED arccoding to the toggled state
    digitalWrite(LED_PIN, led_state);
  }
}
