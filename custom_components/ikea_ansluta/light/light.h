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
  void send_command(Command command);
  void send_command(uint8_t command);
  void add_new_on_change_callback(std::function<void(uint8_t)> &&change_callback);

 protected:
  IkeaAnsluta *parent_;
  bool ignore_state_ = false;
  uint16_t address_{};
  bool pairing_mode_{false};
  optional<float> threshold_;
  light::LightState *state_{nullptr};
  void handle_remote_command_(Command command);
  CallbackManager<void(uint8_t)> on_change_callback_{};
};

class LightOnChangeTrigger : public Trigger<uint8_t> {
 public:
  LightOnChangeTrigger(Light *parent) {
    parent->add_new_on_change_callback([this](uint8_t command) { this->trigger(command); });
  }
};

}  // namespace ikea_ansluta
}  // namespace esphome
