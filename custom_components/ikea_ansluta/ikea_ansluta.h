#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "unordered_map"

namespace esphome {
namespace ikea_ansluta {
enum class IkeaAnslutaCommand : uint8_t {
  OFF = 0x01,
  ON_50 = 0x02,
  ON_100 = 0x03,
  PAIR = 0xFF,
};

struct IkeaAnslutaListener {
  uint16_t remote_address;
  std::function<void(IkeaAnslutaCommand)> on_command;
};

struct IkeaAnslutaCommandState {
  IkeaAnslutaCommand command;
  uint16_t times_sent;
};

class IkeaAnsluta : public Component,
                    public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING,
                                          spi::DATA_RATE_4MHZ> {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void queue_command(uint16_t addr, IkeaAnslutaCommand command);
  void register_listener(uint16_t remote_address, const std::function<void(IkeaAnslutaCommand)> &func);
  void set_send_command_times(uint16_t n_times) { this->send_command_times_ = n_times; }
  void set_sniff_after_commands_sent(uint16_t n) { this->sniff_after_commands_sent_ = n; }

 protected:
  void sniff_();
  void send_strobe_(uint8_t strobe);
  void write_reg_(uint8_t addr, uint8_t value);
  uint8_t read_reg_(uint8_t addr);
  std::vector<uint8_t> read_packet_();
  bool valid_packet_(std::vector<uint8_t>);
  bool valid_cmd_(IkeaAnslutaCommand cmd);
  optional<uint16_t> send_command_times_;
  optional<uint16_t> sniff_after_commands_sent_;
  uint16_t commands_sent_;
  std::vector<IkeaAnslutaListener> listeners_;
  std::unordered_map<uint16_t, IkeaAnslutaCommandState> commands_to_send_{};
  void send_command_(uint16_t address, IkeaAnslutaCommand command);
};
}  // namespace ikea_ansluta
}  // namespace esphome
