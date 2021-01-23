#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ikea_ansluta/ikea_ansluta.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ikea_ansluta {
class IkeaAnslutaSensor : public sensor::Sensor, public Component {
 public:
  void setup() override;
  void dump_config() override;
  void set_parent(IkeaAnsluta *radio) { this->parent_ = radio; };
  void set_address(uint16_t address) { this->address_ = address; };

 protected:
  IkeaAnsluta *parent_;
  bool remote_pressed_ = false;
  uint16_t address_{};
};
}  // namespace ikea_ansluta
}  // namespace esphome
