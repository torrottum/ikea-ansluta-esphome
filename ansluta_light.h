#include "esphome.h"

class Ansluta : public Component, public CustomMQTTDevice
{
private:
  Radio *radio = new Radio();
  std::string topic_prefix = "ikea_ansluta/light";
  bool sending = false;

public:
  void setup() override
  {
    radio->setup();
    subscribe(topic_prefix + "/+/set", &Ansluta::on_set);
  }

  void loop()
  {
    if (sending)
    {
      return;
    }

    short addr = 0;
    char cmd = 0;
    if (radio->sniffCommand(addr, cmd))
    {
      on_remote_command(addr, cmd);
    }

    delay(100);
  }

  void on_remote_command(short address, char cmd)
  {
    if (cmd != LIGHT_OFF)
    {
      publishState(address, "ON");
    }
    else
    {
      publishState(address, "OFF");
    }
  }

  void getRemoteAddressFromTopic(const std::string &topic, short &address)
  {
    address = (short)strtol(topic.substr(topic_prefix.length() + 1, 4).c_str(), NULL, 16);
  }

  void on_set(const std::string &topic, const std::string &payload)
  {
    ESP_LOGD("ansluta", "Topic: '%s', Payload: '%s'", topic.c_str(), payload.c_str());
    short address;
    getRemoteAddressFromTopic(topic, address);
    publishState(address, payload);
    if (payload == "ON")
    {
      radio->sendCommand(address, LIGHT_ON_100);
    }
    else
    {
      radio->sendCommand(address, LIGHT_OFF);
    }
    ESP_LOGD("ansluta", "Remote address: %04x", address);
  }

  void publishState(short address, std::string payload)
  {
    int len = topic_prefix.length() + 12;
    char topic[len];
    snprintf(topic, len, "%s/%04x/state", topic_prefix.c_str(), address);
    ESP_LOGD("ansluta", "Publishing '%s' to '%s'", payload.c_str(), topic);
    publish(topic, payload);
  }
};
