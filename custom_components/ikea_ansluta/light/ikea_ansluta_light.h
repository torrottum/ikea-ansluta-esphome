#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ikea_ansluta/ikea_ansluta.h"
#include "esphome/components/light/light_output.h"
#include "esphome/core/helpers.h"

namespace esphome
{
  namespace ikea_ansluta
  {

    class IkeaAnslutaLight : public Component, public light::LightOutput
    {
    public:
      void setup() override;
      void dump_config() override;
      void set_radio(IkeaAnsluta *radio) { this->radio_ = radio; }
      void set_remote_address(uint16_t address) { this->remote_address_ = address; }
      void set_address(uint16_t address) { this->address_ = address; }
      light::LightTraits get_traits() override;
      void setup_state(light::LightState *state) override;
      void write_state(light::LightState *state) override;

    protected:
      IkeaAnsluta *radio_;
      bool remote_pressed_ = false;
      uint16_t remote_address_{};
      optional<uint16_t> address_{};
      light::LightState *state_{nullptr};
      uint8_t get_command_address() {
        return this->address_.has_value() ? *this->address_ : this->remote_address_;
      };
    };

  } // namespace ikea_ansluta
} // namespace esphome
