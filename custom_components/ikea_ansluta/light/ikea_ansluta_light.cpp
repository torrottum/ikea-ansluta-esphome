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
  ESP_LOGCONFIG(TAG, "  Pairing enabled: %s", ONOFF(pairing_mode_));
}

void IkeaAnslutaLight::setup_state(light::LightState *state) {
  state_ = state;
  state_->set_gamma_correct(0);
  state_->set_default_transition_length(0);
}

light::LightTraits IkeaAnslutaLight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supports_brightness(true);
  return traits;
}

void IkeaAnslutaLight::handle_remote_command_(IkeaAnslutaCommand command) {
  // If we get commands from the remote, we don't want to send commands
  this->ignore_state_ = true;

  this->on_change_callback_.call((uint8_t) command);

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
  call.perform();
}

void IkeaAnslutaLight::write_state(light::LightState *state) {
  if (this->ignore_state_) {
    this->ignore_state_ = false;
    return;
  }

  float brightness;
  state->current_values_as_brightness(&brightness);

  float threshold = this->threshold_.value_or(0.5);
  IkeaAnslutaCommand command;
  if (brightness > 0 && brightness <= threshold) {
    command = IkeaAnslutaCommand::ON_50;
  } else if (brightness > threshold) {
    command = IkeaAnslutaCommand::ON_100;
  } else {
    command = IkeaAnslutaCommand::OFF;
  }
  this->parent_->queue_command(this->address_, command);

  this->on_change_callback_.call((uint8_t) command);
}

void IkeaAnslutaLight::set_pairing_mode(bool pairing_mode) {
  ESP_LOGI(TAG, "Pairing mode %s for address %#04x", ONOFF(pairing_mode), this->address_);
  if (!pairing_mode) {
    this->cancel_interval("pairing_mode_");
    return;
  }
  this->set_interval("pairing_mode_", 5000, [this]() { this->send_pairing_command(); });
}

void IkeaAnslutaLight::send_pairing_command() {
  ESP_LOGI(TAG, "Sending pairing command with address %#04x", this->address_);
  this->send_command(IkeaAnslutaCommand::PAIR);
}

void IkeaAnslutaLight::send_command(uint8_t command) { this->send_command((IkeaAnslutaCommand)((uint8_t) command)); }

void IkeaAnslutaLight::send_command(IkeaAnslutaCommand command) {
  this->parent_->queue_command(this->address_, command);
}

void IkeaAnslutaLight::add_new_on_change_callback(std::function<void(uint8_t)> &&change_callback) {
  this->on_change_callback_.add(std::move(change_callback));
}
}  // namespace ikea_ansluta
}  // namespace esphome
