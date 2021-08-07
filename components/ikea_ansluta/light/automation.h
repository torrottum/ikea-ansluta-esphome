#include "esphome/core/automation.h"

namespace esphome {
namespace ikea_ansluta {
class LightOnChangeTrigger : public Trigger<uint8_t> {
 public:
  LightOnChangeTrigger(Light *parent) {
    parent->add_on_change_callback([this](uint8_t command) { this->trigger(command); });
  }
};
} // namespace ikea_ansluta
} // namespace esphome