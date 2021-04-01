#pragma once
#include "esphome/core/log.h"
#include "esphome/core/automation.h"
#include "esphome/components/light/light_state.h"
#include "ikea_ansluta.h"
#include "light/light.h"

namespace esphome {
namespace ikea_ansluta {
class OnRemoteClickTrigger : public Trigger<uint16_t, uint8_t> {
 public:
  OnRemoteClickTrigger(IkeaAnsluta *parent) {
    parent->add_new_on_remote_click_callback(
        [this](uint16_t address, uint8_t command) { this->trigger(address, command); });
  }
};

template<typename... Ts> class EnablePairingModeAction : public Action<Ts...> {
 public:
  explicit EnablePairingModeAction(light::LightState *state) : light_state_(state) {}

  void play(Ts... x) override {
    ((ikea_ansluta::Light*) this->light_state_->get_output())->set_pairing_mode(true);
  }

 protected:
  light::LightState *light_state_;
};

template<typename... Ts> class DisablePairingModeAction : public Action<Ts...> {
 public:
  explicit DisablePairingModeAction(light::LightState *state) : light_state_(state) {}

  void play(Ts... x) override {
    ((ikea_ansluta::Light*) this->light_state_->get_output())->set_pairing_mode(false);
  }

 protected:
  light::LightState *light_state_;
};

template<typename... Ts> class SendCommandAction : public Action<Ts...> {
 public:
  explicit SendCommandAction(light::LightState *state) : light_state_(state) {}

 TEMPLATABLE_VALUE(uint8_t, command)

  void play(Ts... x) override {
    ((ikea_ansluta::Light*) this->light_state_->get_output())->send_command(command_.value(x...));
  }

 protected:
  light::LightState *light_state_;
};

}  // namespace ikea_ansluta
}  // namespace esphome