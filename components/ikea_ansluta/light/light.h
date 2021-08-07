#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/light/light_traits.h"
#include "esphome/components/ikea_ansluta/ikea_ansluta.h"
#include "esphome/components/light/light_output.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ikea_ansluta {

class Light : public Component, public light::LightOutput {
 public:
  void setup() override;
  void dump_config() override;
  void setup_state(light::LightState *state) override;
  void write_state(light::LightState *state) override;
  light::LightTraits get_traits() override;
  void set_parent(IkeaAnsluta *parent) { this->parent_ = parent; };
  void set_address(uint16_t address) { this->address_ = address; };
  void set_pairing_mode(bool pairing_mode);
  void set_threshold(float threshold) { this->threshold_ = threshold; };
  void send_pairing_command();
  void add_on_change_callback(std::function<void(uint8_t)> &&change_callback);

 protected:
  IkeaAnsluta *parent_;
  light::LightState *state_{nullptr};
  uint16_t address_{};
  bool pairing_mode_enabled_{false};
  optional<float> threshold_{0.5};
  bool ignore_state_{false};
  CallbackManager<void(uint8_t)> on_change_callback_{};
  void handle_remote_command_(Command command);
};
}  // namespace ikea_ansluta
}  // namespace esphome
