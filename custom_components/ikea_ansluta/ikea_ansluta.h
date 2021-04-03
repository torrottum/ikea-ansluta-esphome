#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "unordered_map"

namespace esphome {
namespace ikea_ansluta {
enum class Command : uint8_t {
  OFF = 0x01,
  ON_50 = 0x02,
  ON_100 = 0x03,
  PAIR = 0xFF,
};

struct RemoteListener {
  uint16_t address;
  std::function<void(Command)> on_command;
};

struct SendingCommandState {
  Command command;
  uint16_t times_sent;
};


class IkeaAnsluta : public Component,
                    public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING,
                        spi::DATA_RATE_4MHZ> {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void queue_command(uint16_t address, Command command);
  void add_on_remote_click_callback(std::function<void(uint16_t, uint8_t)> &&remote_pressed_callback);
  void set_send_command_times(uint16_t n_times) { this->send_command_times_ = n_times; }
  void set_sniff_after_command_sent_x_times(uint16_t times) { this->sniff_after_command_sent_x_times_ = times; }
 protected:
  void write_reg_(uint8_t address, uint8_t value);
  void send_strobe_(uint8_t strobe);
  uint8_t read_reg_(uint8_t address);
  void sniff_();
  void read_packet_(std::vector<uint8_t> &buffer);
  bool valid_packet_(const std::vector<uint8_t> &packet);
  bool valid_cmd_(Command cmd);
  void send_command_(uint16_t address, Command command);
  std::unordered_map<uint16_t, SendingCommandState> command_queue_;
  uint16_t commands_sent_ = 0;
  CallbackManager<void(uint16_t, uint8_t)> on_remote_click_callback_{};
  optional<uint16_t> send_command_times_;
  optional<uint16_t> sniff_after_command_sent_x_times_;
};
}  // namespace ikea_ansluta
}  // namespace esphome