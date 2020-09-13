#include "esphome.h"

class Ansluta : public Component, public CustomMQTTDevice
{
private:
  std::string topic_prefix = "ikea_ansluta/light";
public:
  void setup() override
  {
    subscribe(topic_prefix+"/+/set", &Ansluta::on_set);
  }

  void loop() {
  }

  void on_set(const std::string &topic, const std::string &payload)
  {
    ESP_LOGD("ansluta", "Topic: '%s', Payload: '%s'", topic.c_str(), payload.c_str());
  }
};
