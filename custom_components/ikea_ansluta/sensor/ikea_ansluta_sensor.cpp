#include "esphome/core/log.h"
#include "ikea_ansluta_sensor.h"

namespace esphome {
namespace ikea_ansluta {
static const char *TAG = "ikea_ansluta.sensor";

void IkeaAnslutaSensor::setup() {
  this->radio_->register_listener(this->remote_address_, [this](IkeaAnslutaCommand command) {
    this->publish_state((uint8_t) command);
  });
}

void IkeaAnslutaSensor::dump_config() {
  LOG_SENSOR("", "Ikea Ansluta Remote Sensor", this);
  ESP_LOGCONFIG(TAG, "  Remote address: %#04x", this->remote_address_);
}
}  // namespace ikea_ansluta
}  // namespace esphome
