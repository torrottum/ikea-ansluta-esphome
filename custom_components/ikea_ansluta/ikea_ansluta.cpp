#include "esphome/core/log.h"
#include "ikea_ansluta.h"
#include "cc2500.h"

namespace esphome {
namespace ikea_ansluta {
static const char *TAG = "ikea_ansluta";

void IkeaAnsluta::setup() {
  this->spi_setup();
  this->enable();
  this->send_strobe_(CC2500_SRES);
  this->write_reg_(REG_IOCFG2, VAL_IOCFG2);
  this->write_reg_(REG_IOCFG0, VAL_IOCFG0);
  this->write_reg_(REG_PKTLEN, VAL_PKTLEN);
  this->write_reg_(REG_PKTCTRL1, VAL_PKTCTRL1);
  this->write_reg_(REG_PKTCTRL0, VAL_PKTCTRL0);
  this->write_reg_(REG_ADDR, VAL_ADDR);
  this->write_reg_(REG_CHANNR, VAL_CHANNR);
  this->write_reg_(REG_FSCTRL1, VAL_FSCTRL1);
  this->write_reg_(REG_FSCTRL0, VAL_FSCTRL0);
  this->write_reg_(REG_FREQ2, VAL_FREQ2);
  this->write_reg_(REG_FREQ1, VAL_FREQ1);
  this->write_reg_(REG_FREQ0, VAL_FREQ0);
  this->write_reg_(REG_MDMCFG4, VAL_MDMCFG4);
  this->write_reg_(REG_MDMCFG3, VAL_MDMCFG3);
  this->write_reg_(REG_MDMCFG2, VAL_MDMCFG2);
  this->write_reg_(REG_MDMCFG1, VAL_MDMCFG1);
  this->write_reg_(REG_MDMCFG0, VAL_MDMCFG0);
  this->write_reg_(REG_DEVIATN, VAL_DEVIATN);
  this->write_reg_(REG_MCSM2, VAL_MCSM2);
  this->write_reg_(REG_MCSM1, VAL_MCSM1);
  this->write_reg_(REG_MCSM0, VAL_MCSM0);
  this->write_reg_(REG_FOCCFG, VAL_FOCCFG);
  this->write_reg_(REG_BSCFG, VAL_BSCFG);
  this->write_reg_(REG_AGCCTRL2, VAL_AGCCTRL2);
  this->write_reg_(REG_AGCCTRL1, VAL_AGCCTRL1);
  this->write_reg_(REG_AGCCTRL0, VAL_AGCCTRL0);
  this->write_reg_(REG_WOREVT1, VAL_WOREVT1);
  this->write_reg_(REG_WOREVT0, VAL_WOREVT0);
  this->write_reg_(REG_WORCTRL, VAL_WORCTRL);
  this->write_reg_(REG_FREND1, VAL_FREND1);
  this->write_reg_(REG_FREND0, VAL_FREND0);
  this->write_reg_(REG_FSCAL3, VAL_FSCAL3);
  this->write_reg_(REG_FSCAL2, VAL_FSCAL2);
  this->write_reg_(REG_FSCAL1, VAL_FSCAL1);
  this->write_reg_(REG_FSCAL0, VAL_FSCAL0);
  this->write_reg_(REG_RCCTRL1, VAL_RCCTRL1);
  this->write_reg_(REG_RCCTRL0, VAL_RCCTRL0);
  this->write_reg_(REG_FSTEST, VAL_FSTEST);
  this->write_reg_(0x3E, 0xFF);
  this->disable();
}

void IkeaAnsluta::dump_config() {
  ESP_LOGCONFIG(TAG, "Ikea Ansluta:");
  LOG_PIN("  CS PIN:", this->cs_);
  if (this->send_command_times_.has_value())
    ESP_LOGCONFIG(TAG, "  Send command times: %d", this->send_command_times_.value());
  if (this->sniff_after_command_sent_x_times_.has_value())
    ESP_LOGCONFIG(TAG, "  Sniff after command sent X times: %d", this->sniff_after_command_sent_x_times_.value());
}

void IkeaAnsluta::loop() {
  this->enable();
  // Sniffing slows down command sending. Therefore we only sniff if we are
  // not sending commands or for after we have sent x commands
  if (this->command_queue_.empty()) {
    this->sniff_();
  } else if (this->commands_sent_ % this->sniff_after_command_sent_x_times_.value_or(5) == 0) {
    this->sniff_();
    this->commands_sent_ = 0;
  }

  for (auto it = this->command_queue_.begin(); it != this->command_queue_.end();) {
    this->send_command_(it->first, it->second.command);
    it->second.times_sent++;
    this->commands_sent_++;
    if (it->second.times_sent == this->send_command_times_.value_or(50)) {
      ESP_LOGD(TAG, "Done sending command %#04x to address %#04x", (uint8_t) it->second.command, it->first);
      it = this->command_queue_.erase(it);
    } else {
      ++it;
    }
  }
  this->disable();
}

void IkeaAnsluta::write_reg_(uint8_t address, uint8_t value) {
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(address);
  delayMicroseconds(200);
  this->write_byte(value);
  delayMicroseconds(1);
  this->cs_->digital_write(true);
}

uint8_t IkeaAnsluta::read_reg_(uint8_t address) {
  address += 0x80;
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(address);
  delay(10);
  uint8_t value = this->read_byte();
  delayMicroseconds(1);
  this->cs_->digital_write(true);
  return value;
}

void IkeaAnsluta::send_strobe_(uint8_t strobe) {
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(strobe);
  this->cs_->digital_write(true);
  delayMicroseconds(2000);
}

void IkeaAnsluta::sniff_() {
  std::vector<uint8_t> packet;
  this->read_packet_(packet);
  if (this->valid_packet_(packet)) {
    uint16_t address = (packet[2] << 8) + packet[3];
    Command command = (Command) packet[4];
    ESP_LOGV(TAG, "Sniffed command %#02x from remote %#04x", (uint8_t) command, address);
    this->on_remote_click_callback_.call(address, (uint8_t) command);
  }
}

void IkeaAnsluta::queue_command(uint16_t address, Command command) {
  ESP_LOGV(TAG, "Queued command %#02x to address %#04x", address, (uint8_t) command);
  if (!this->command_queue_.count(address)) {
    auto pair = std::make_pair(address, SendingCommandState{
        .command = command,
        .times_sent = 0,
    });

    this->command_queue_.insert(pair);
    return;
  }

  if (this->command_queue_[address].command != command) {
    this->command_queue_[address].command = command;
    this->command_queue_[address].times_sent = 0;
  }
}

void IkeaAnsluta::read_packet_(std::vector<uint8_t> &buffer) {
  this->send_strobe_(CC2500_SRX);
  this->write_reg_(REG_IOCFG1, 0x01);
  delay(20);

  uint8_t len = this->read_reg_(CC2500_FIFO);
  if (len == 6) {
    for (int i = 0; i < 6; i++) {
      buffer.push_back(this->read_reg_(CC2500_FIFO));
    }
  }
  this->send_strobe_(CC2500_SIDLE);
  this->send_strobe_(CC2500_SFRX);
}

bool IkeaAnsluta::valid_packet_(const std::vector<uint8_t> &packet) {
  return packet.size() == 6 && packet.front() == 0x55 && packet.at(1) == 0x01 &&
         this->valid_cmd_((Command) packet.at(4)) && packet.back() == 0xAA;
}

bool IkeaAnsluta::valid_cmd_(Command cmd) {
  return cmd == Command::OFF || cmd == Command::ON_50 || cmd == Command::ON_100 ||
         cmd == Command::PAIR;
}

void IkeaAnsluta::send_command_(uint16_t address, Command command) {
  this->send_strobe_(CC2500_SFTX);   // 0x3B
  this->send_strobe_(CC2500_SIDLE);  // 0x36
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte(0x7F);
  this->write_byte(0x06);
  this->write_byte(0x55);
  this->write_byte(0x01);
  this->write_byte((uint8_t)(address >> 8));
  this->write_byte((uint8_t)(address & 0xFF));
  this->write_byte((uint8_t) command);
  this->write_byte(0xAA);
  this->write_byte(0xFF);
  this->cs_->digital_write(true);
  this->send_strobe_(CC2500_STX);
  delayMicroseconds(10);
}

void IkeaAnsluta::add_on_remote_click_callback(std::function<void(uint16_t, uint8_t)> &&remote_pressed_callback) {
  this->on_remote_click_callback_.add(std::move(remote_pressed_callback));
}
}
}