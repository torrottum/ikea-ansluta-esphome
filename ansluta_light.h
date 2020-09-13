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

  char hexdigit(char hex)
  {
    return (hex <= '9') ? hex - '0' : toupper(hex) - 'A' + 10;
  }

  char hexbyte(const char *hex)
  {
    return (hexdigit(*hex) << 4) | hexdigit(*(hex + 1));
  }

  void getRemoteAddressFromTopic(const std::string &topic, char &addrA, char &addrB)
  {
    addrA = hexbyte(topic.substr(topic_prefix.length()+1, 2).c_str());
    addrB = hexbyte(topic.substr(topic_prefix.length()+3, 2).c_str());
  }

  void on_set(const std::string &topic, const std::string &payload)
  {
    ESP_LOGD("ansluta", "Topic: '%s', Payload: '%s'", topic.c_str(), payload.c_str());
    char addrA;
    char addrB;
    getRemoteAddressFromTopic(topic, addrA, addrB);
    ESP_LOGD("ansluta", "Remote address: %#2x %#2x", addrA, addrB);
  }
};
