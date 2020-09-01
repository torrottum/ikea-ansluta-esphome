#include <IotWebConf.h>
#include <IotWebConfCompatibility.h>

#ifdef ESP8266
String ChipId = String(ESP.getChipId(), HEX);
#elif ESP32
String ChipId = String((uint32_t)ESP.getEfuseMac(), HEX);
#endif


String thingName = String("Ikea-Ansluta-MQTT-Bridge-") + ChipId;
const char wifiInitialApPassword[] = "ikea-ansluta";

#define STRING_LEN 128

#define CONFIG_VERSION "1"

#define STATUS_PIN LED_BUILTIN

DNSServer dnsServer;
WebServer server(80);

char mqttServerValue[STRING_LEN];
char mqttUserNameValue[STRING_LEN];
char mqttUserPasswordValue[STRING_LEN];

IotWebConf iotWebConf(thingName.c_str(), &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
IotWebConfParameter mqttServerParam = IotWebConfParameter("MQTT server", "mqttServer", mqttServerValue, STRING_LEN);
IotWebConfParameter mqttUserNameParam = IotWebConfParameter("MQTT user", "mqttUser", mqttUserNameValue, STRING_LEN);
IotWebConfParameter mqttUserPasswordParam = IotWebConfParameter("MQTT password", "mqttPass", mqttUserPasswordValue, STRING_LEN, "password");

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  iotWebConf.setStatusPin(STATUS_PIN);
  iotWebConf.addParameter(&mqttServerParam);
  iotWebConf.addParameter(&mqttUserNameParam);
  iotWebConf.addParameter(&mqttUserPasswordParam);
  iotWebConf.init();

  server.on("/", handleRoot);
  server.on("/config", [] { iotWebConf.handleConfig(); });
  server.on("/ping", [] { server.send(200, "text/plain", "pong"); });
  server.onNotFound([]() { iotWebConf.handleNotFound(); });

  Serial.println("Ready.");
}

void loop()
{
  iotWebConf.doLoop();
}

void handleRoot()
{
  server.send(200, "text/html", "");
}
