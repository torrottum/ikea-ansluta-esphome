#include "esphome/core/log.h"
#include "light.h"

namespace esphome {
namespace ikea_ansluta {
static const char *TAG = "ikea_ansluta.light";

void Light::setup() {
  this->parent_->add_on_remote_click_callback(
      [this](uint16_t address, uint8_t command) {
        // Remote and light and has same addr, i.e not paired directly with the light
        if (this->address_ == address) {
          this->handle_remote_command_((Command) command);
        }
      });
}

void Light::dump_config() {
  ESP_LOGCONFIG(TAG, "  Address: %#04x", this->address_);
  ESP_LOGCONFIG(TAG, "  Pairing mode enabled: %s", ONOFF(this->pairing_mode_enabled_));
  if (this->threshold_.has_value())
    ESP_LOGCONFIG(TAG, "  Threshold: %.2f", this->threshold_.value());
}

void Light::setup_state(light::LightState *state) {
  state_ = state;
  state_->set_gamma_correct(0);
  state_->set_default_transition_length(0);
}

light::LightTraits Light::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supports_brightness(true);
  return traits;
}

void Light::handle_remote_command_(Command command) {
  ESP_LOGW(TAG, "Your remote and light has the same address. "
                "It's recommended to pair with the lights directly for the best experience. "
                "Read the wiki for more info");

  // If we get commands from the remote, we don't want to send commands
  this->ignore_state_ = true;

  auto call = this->state_->make_call();
  switch (command) {
    case Command::ON_50:
      call.set_brightness(this->threshold_.value_or(0.5));
      call.set_state(true);
      break;
    case Command::ON_100:
      call.set_brightness(1.0f);
      call.set_state(true);
      break;
    case Command::OFF:
      call.set_state(false);
      break;
    case Command::PAIR:
      // Ignore pairing commands
      break;
  }
  call.perform();

  if (command != Command::PAIR)
    on_change_callback_.call((uint8_t) command);
}

void Light::write_state(light::LightState *state) {
  if (this->ignore_state_) {
    this->ignore_state_ = false;
    return;
  }

  float brightness;
  state->current_values_as_brightness(&brightness);

  float threshold = this->threshold_.value_or(0.5);
  Command command;
  if (brightness > 0 && brightness <= threshold) {
    command = Command::ON_50;
  } else if (brightness > threshold) {
    command = Command::ON_100;
  } else {
    command = Command::OFF;
  }
  on_change_callback_.call((uint8_t) command);
  this->parent_->queue_command(this->address_, command);
}

void Light::set_pairing_mode(bool pairing_enabled) {
  ESP_LOGI(TAG, "Pairing mode %s for address %#04x: ", ONOFF(pairing_enabled), this->address_);
  if (!pairing_enabled) {
    this->cancel_interval("pairing_mode_");
    return;
  }
  this->set_interval("pairing_mode_", 5000, [this]() { this->send_pairing_command(); });
}

void Light::send_pairing_command() {
  ESP_LOGI(TAG, "Sending pairing command with address %#04x", this->address_);
  this->parent_->queue_command(this->address_, Command::PAIR);
}

void Light::add_on_change_callback(std::function<void(uint8_t)> &&change_callback) {
  this->on_change_callback_.add(std::move(change_callback));
}
}  // namespace ikea_ansluta
}  // namespace esphome
