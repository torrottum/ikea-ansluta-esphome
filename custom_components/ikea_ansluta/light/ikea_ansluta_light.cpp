#include "esphome/core/log.h"
#include "ikea_ansluta_light.h"

namespace esphome {
namespace ikea_ansluta {
static const char *TAG = "ikea_ansluta.light";

void IkeaAnslutaLight::setup() {
  this->parent_->register_listener(this->address_, [this](IkeaAnslutaCommand command) {
    ESP_LOGV(TAG, "Received command %#04x from radio", (uint8_t) command);
    this->handle_remote_command_(command);
  });
}

void IkeaAnslutaLight::dump_config() {
  ESP_LOGCONFIG(TAG, "  Address: %#04x", this->address_);
  ESP_LOGCONFIG(TAG, "  Pairing enabled: %s", this->pairing_mode ? "true" : "false");
}

light::LightTraits IkeaAnslutaLight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supports_brightness(true);
  return traits;
}

void IkeaAnslutaLight::setup_state(light::LightState *state) {
  state_ = state;
  state_->set_gamma_correct(0);
  state_->set_default_transition_length(0);
}

void IkeaAnslutaLight::handle_remote_command_(IkeaAnslutaCommand command) {
  auto call = this->state_->make_call();
  switch (command) {
    case IkeaAnslutaCommand::ON_50:
      call.set_brightness(0.5f);
      call.set_state(true);
      break;
    case IkeaAnslutaCommand::ON_100:
      call.set_brightness(1.0f);
      call.set_state(true);
      break;
    case IkeaAnslutaCommand::OFF:
      call.set_state(false);
      break;
    case IkeaAnslutaCommand::PAIR:
      // Ignore pairing commands
      break;
  }

  this->remote_pressed_ = true;
  call.perform();
}

void IkeaAnslutaLight::write_state(light::LightState *state) {
  // Do not write state if set from remote
  if (this->remote_pressed_) {
    this->remote_pressed_ = false;
    return;
  }

  float brightness;
  state->current_values_as_brightness(&brightness);

  // TODO: make threshold configurable
  if (brightness > 0 && brightness <= 0.5) {
    this->parent_->send_command(this->address_, IkeaAnslutaCommand::ON_50);
  } else if (brightness > 0.5) {
    this->parent_->send_command(this->address_, IkeaAnslutaCommand::ON_100);
  } else {
    this->parent_->send_command(this->address_, IkeaAnslutaCommand::OFF);
  }
}
}  // namespace ikea_ansluta
}  // namespace esphome
