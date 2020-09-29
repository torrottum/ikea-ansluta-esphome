#include "esphome/core/log.h"
#include "ikea_ansluta_light.h"

namespace esphome
{
  namespace ikea_ansluta
  {
    static const char *TAG = "ikea_ansluta.light";

    void IkeaAnslutaLight::setup() {
      this->radio_->register_listener(this->remote_address_, [this](IkeaAnslutaCommand command) {
        ESP_LOGD(TAG, "Received command %#04x from radio", command);
        this->remote_pressed_ = true;
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
          return;
          break;
        }
        call.perform();
      });
    }

    void IkeaAnslutaLight::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Remote ID: %#04x", this->remote_address_);
    }

    light::LightTraits IkeaAnslutaLight::get_traits()
    {
      auto traits = light::LightTraits();
      traits.set_supports_brightness(true);
      return traits;
    }

    void IkeaAnslutaLight::setup_state(light::LightState *state) {
      state_ = state;
      state_->set_gamma_correct(0);
      state_->set_default_transition_length(0);
    }

    void IkeaAnslutaLight::write_state(light::LightState *state)
    {
      // Do not write state if set from remote
      if (this->remote_pressed_) {
        this->remote_pressed_ = false;
        return;
      }

      float brightness;
      state->current_values_as_brightness(&brightness);

      // TODO: make threshold configurable
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
