#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ikea_ansluta/ikea_ansluta.h"
#include "esphome/components/light/light_output.h"

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
      light::LightTraits get_traits() override;
      void setup_state(light::LightState *state) override;
      void write_state(light::LightState *state) override;

    protected:
      IkeaAnsluta *radio_;
      bool remote_pressed_ = false;
      uint16_t remote_address_{};
      light::LightState *state_{nullptr};
    };

  } // namespace ikea_ansluta
} // namespace esphome
