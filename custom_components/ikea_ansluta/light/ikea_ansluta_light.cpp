#include "esphome/core/log.h"
#include "ikea_ansluta_light.h"

namespace esphome
{
  namespace ikea_ansluta
  {
    static const char *TAG = "ikea_ansluta.light";

    void IkeaAnslutaLight::setup()
    {
      this->radio_->register_listener(this->remote_address_, [this](IkeaAnslutaCommand command) {
        ESP_LOGV(TAG, "Received command %#04x from radio", command);
        this->handle_remote_command(command);
      });
    }

    void IkeaAnslutaLight::loop()
    {
      if (this->send_pairing_command_at_ == 0)
        return;

      if (millis() >= this->send_pairing_command_at_)
      {
        ESP_LOGI(TAG, "Sending pairing command ...");
        this->send_command(IkeaAnslutaCommand::PAIR);
        ESP_LOGI(TAG, "Pairing command sent!");
        this->send_pairing_command_at_ = 0;
      }
    }

    void IkeaAnslutaLight::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Remote address: %#04x", this->remote_address_);
      if (this->address_.has_value())
        ESP_LOGCONFIG(TAG, "Address: %#04x", *this->address_);
      ESP_LOGCONFIG(TAG, "Pairing enabled: %s", this->pairing_enabled_ ? "true" : "false");
    }

    light::LightTraits IkeaAnslutaLight::get_traits()
    {
      auto traits = light::LightTraits();
      traits.set_supports_brightness(true);
      return traits;
    }

    void IkeaAnslutaLight::setup_state(light::LightState *state)
    {
      state_ = state;
      state_->set_gamma_correct(0);
      state_->set_default_transition_length(0);
    }

    void IkeaAnslutaLight::handle_remote_command(IkeaAnslutaCommand command)
    {
      auto call = this->state_->make_call();
      switch (command)
      {
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
        if (!this->pairing_enabled_)
          return;

        // TODO: Should be possible to validate this in the config?
        if (!this->address_.has_value()) {
          ESP_LOGW(TAG, "Pairing mode enabled, but address is not set!");
          return;
        }

        if (this->send_pairing_command_at_ > 0)
          return;

        ESP_LOGI(TAG, "Waiting ~5s before sending pairing commmand ...");
        // Command is sent in loop()
        this->send_pairing_command_at_ = millis() + 5000;
        return;
      }

      this->remote_pressed_ = true;
      call.perform();
    }

    void IkeaAnslutaLight::send_command(IkeaAnslutaCommand command)
    {
      auto address = this->address_.has_value() ? *this->address_ : this->remote_address_;
      this->radio_->send_command(address, command);
    }

    void IkeaAnslutaLight::write_state(light::LightState *state)
    {
      // Do not write state if set from remote
      if (this->remote_pressed_)
      {
        this->remote_pressed_ = false;
        return;
      }

      float brightness;
      state->current_values_as_brightness(&brightness);

      // TODO: make threshold configurable
      // TODO: refactor to use this->send_command()
      if (brightness > 0 && brightness <= 0.5)
      {
        this->radio_->send_command(this->remote_address_, IkeaAnslutaCommand::ON_50);
      }
      else if (brightness > 0.5)
      {
        this->radio_->send_command(this->remote_address_, IkeaAnslutaCommand::ON_100);
      }
      else
      {
        this->radio_->send_command(this->remote_address_, IkeaAnslutaCommand::OFF);
      }
    }
  } // namespace ikea_ansluta
} // namespace esphome
