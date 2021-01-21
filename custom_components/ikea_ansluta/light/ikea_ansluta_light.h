#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ikea_ansluta/ikea_ansluta.h"
#include "esphome/components/light/light_output.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ikea_ansluta {

class IkeaAnslutaLight : public Component, public light::LightOutput {
 public:
  void setup() override;
  void dump_config() override;
  void set_parent(IkeaAnsluta *parent) { this->parent_ = parent; };
  void set_address(uint16_t address) { this->address_ = address; };
  void set_pairing_enabled(bool allow) { this->pairing_enabled_ = allow; };
  light::LightTraits get_traits() override;
  void setup_state(light::LightState *state) override;
  void write_state(light::LightState *state) override;

 protected:
  IkeaAnsluta *parent_;
  bool remote_pressed_ = false;
  uint16_t address_{};
  bool pairing_enabled_{false};
  uint32_t send_pairing_command_at_{0};
  light::LightState *state_{nullptr};
  void handle_remote_command_(IkeaAnslutaCommand command);
};

}  // namespace ikea_ansluta
}  // namespace esphome
