/*
 *   Tested with "WiFi Smart Socket ESP8266 MQTT"
 *   and "Sonoff - WiFi Wireless Smart Switch ESP8266 MQTT"
 *
 *   The Relay could be toggeled with the physical pushbutton
*/

#include <Homie.h> // this is based on homie 2.0

const int PIN_RELAY = 12;
const int PIN_LED   = 13;
const int PIN_BUTTON = 0;

unsigned long buttonDownTime = 0;
byte lastButtonState = 1;
byte buttonPressHandled = 0;

HomieNode switchNode      ("switchDoor" , "switch"); // ID, type
HomieNode temperatureNode ("temperature", "temperature");
HomieNode humidityNode    ("humidity"   , "humidity");
// HomieNode temperatureNode("basementtemperature", "temperature");

bool switchOnHandler(HomieRange range, String value) {
  if (value != "true" && value != "false") return false;

  bool on = (value == "true");
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  switchNode.setProperty("on").send(value);
  Homie.getLogger() << "Switch is " << (on ? "on" : "off") << "(MQTT)" << endl;

  return true;
}

void toggleRelay() {
  bool on = digitalRead(PIN_RELAY) == HIGH;
  digitalWrite(PIN_RELAY, on ? LOW : HIGH);
  switchNode.setProperty("on").send(on ? "false" : "true");
  Homie.getLogger() << "Switch is " << (on ? "off" : "on") <<"(Button)" << endl;
}

void loopHandler() {
  byte buttonState = digitalRead(PIN_BUTTON);
  if ( buttonState != lastButtonState ) {
    if (buttonState == LOW) {
      buttonDownTime     = millis();
      buttonPressHandled = 0;
    }
    else {
      unsigned long dt = millis() - buttonDownTime;
      if ( dt >= 90 && dt <= 900 && buttonPressHandled == 0 ) {
        toggleRelay();
        buttonPressHandled = 1;
      }
    }
    lastButtonState = buttonState;
  }
}


const int TEMPERATURE_INTERVAL = 1; // 300;
unsigned long lastTemperatureSent = 0;

void setupHandlerTemp() {
  temperatureNode.setProperty("unit").send("c");
}

void loopHandlerTemp() {
  if (millis() - lastTemperatureSent >= TEMPERATURE_INTERVAL * 1000UL || lastTemperatureSent == 0) {
    float temperature = 22; // Fake temperature here, for the example
    Homie.getLogger() << "Temperature: " << temperature << " C" << endl; //°
    temperatureNode.setProperty("degrees").send(String(temperature));
    float humidity =  5; // Fake temperature here, for the example
    Homie.getLogger() << "Humidity   : " << humidity << " C" << endl; //°
    temperatureNode.setProperty("degrees").send(String(humidity));
    lastTemperatureSent = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_RELAY, LOW);

  Homie_setFirmware("garage-doorlock", "1.0.0");
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);
  Homie.setSetupFunction(setupHandlerTemp).setLoopFunction(loopHandlerTemp);

  switchNode.advertise("on").settable(switchOnHandler);
  temperatureNode.advertise("unit");
  temperatureNode.advertise("degrees");
  humidityNode.advertise("unit");
  humidityNode.advertise("degrees");

  Homie.setLoopFunction(loopHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
