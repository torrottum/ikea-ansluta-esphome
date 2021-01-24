#pragma once
#include "esphome/core/automation.h"
#include "ikea_ansluta.h"

namespace esphome {
namespace ikea_ansluta {
class OnRemoteClickTrigger : public Trigger<uint16_t, uint8_t> {
 public:
  OnRemoteClickTrigger(IkeaAnsluta *parent) {
    parent->add_new_on_remote_click_callback(
        [this](uint16_t address, uint8_t command) { this->trigger(address, command); });
  }
};
}  // namespace ikea_ansluta
}  // namespace esphome